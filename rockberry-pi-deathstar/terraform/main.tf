# Death Star Multi-Region Infrastructure

# VPC Peering
resource "aws_vpc_peering_connection" "primary_secondary" {
  vpc_id        = aws_vpc.primary.id # us-east-1
  peer_vpc_id   = aws_vpc.secondary.id # us-west-2
  auto_accept   = true
}

# Global Accelerator
resource "aws_globalaccelerator_accelerator" "main" {
  name            = "death-star-ga"
  ip_address_type = "IPV4"
  enabled         = true
}

resource "aws_globalaccelerator_listener" "main" {
  accelerator_arn = aws_globalaccelerator_accelerator.main.id
  client_affinity = "NONE"
  protocol        = "TCP"

  port_range {
    from_port = 80
    to_port   = 80
  }
}

resource "aws_globalaccelerator_endpoint_group" "primary" {
  listener_arn = aws_globalaccelerator_listener.main.id
  endpoint_group_region = "us-east-1"
  health_check_path = "/healthz"
  health_check_port = 80
  health_check_protocol = "HTTP"
}

resource "aws_globalaccelerator_endpoint_group" "secondary" {
  listener_arn = aws_globalaccelerator_listener.main.id
  endpoint_group_region = "us-west-2"
  health_check_path = "/healthz"
  health_check_port = 80
  health_check_protocol = "HTTP"
}

# DynamoDB Global Tables
resource "aws_dynamodb_table" "targeting_coordinates" {
  name             = "TargetingCoordinates"
  hash_key         = "TargetId"
  billing_mode     = "PAY_PER_REQUEST"

  attribute {
    name = "TargetId"
    type = "S"
  }

  replica {
    region_name = "us-east-1"
  }

  replica {
    region_name = "us-west-2"
  }
}

# EKS/GKE abstraction implies managed kubernetes
resource "aws_eks_cluster" "primary" {
  name     = "death-star-primary-cluster"
  role_arn = aws_iam_role.eks_cluster.arn

  vpc_config {
    subnet_ids = aws_subnet.primary[*].id
  }
}

# Dummy resources for validation
resource "aws_vpc" "primary" {
  cidr_block = "10.0.0.0/16"
}

resource "aws_vpc" "secondary" {
  cidr_block = "10.1.0.0/16"
}

resource "aws_iam_role" "eks_cluster" {
  name = "eks-cluster-role"
  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Action = "sts:AssumeRole"
        Effect = "Allow"
        Principal = {
          Service = "eks.amazonaws.com"
        }
      }
    ]
  })
}

resource "aws_subnet" "primary" {
  count      = 2
  vpc_id     = aws_vpc.primary.id
  cidr_block = cidrsubnet(aws_vpc.primary.cidr_block, 8, count.index)
}
