# Region 1: Coruscant (US-East-1)
module "vpc_east" {
source = "../../modules/vpc"
region = "us-east-1"
cidr_block = "10.1.0.0/16"
env = "prod"
}

# Region 2: Alderaan-Proxy (US-West-2)
module "vpc_west" {
source = "../../modules/vpc"
region = "us-west-2"
cidr_block = "10.2.0.0/16"
env = "prod"
providers = {
aws = aws.west
}
}

resource "aws_vpc_peering_connection" "inter_region" {
peer_vpc_id = var.west_vpc_id
vpc_id = var.east_vpc_id
peer_region = "us-west-2"
auto_accept = false
}

# Accepter side (West)
resource "aws_vpc_peering_connection_accepter" "peer" {
provider = aws.west
vpc_peering_connection_id = aws_vpc_peering_connection.inter_region.id
auto_accept = true
}

resource "aws_globalaccelerator_accelerator" "death_star_ingress" {
name = "death-star-global-endpoint"
ip_address_type = "IPV4"
enabled = true
}

resource "aws_globalaccelerator_listener" "https" {
accelerator_arn = aws_globalaccelerator_accelerator.death_star_ingress.id
port_range {
from_port = 443
to_port = 443
}
protocol = "TCP"
}

# Endpoint Group for US-EAST-1
resource "aws_globalaccelerator_endpoint_group" "east" {
listener_arn = aws_globalaccelerator_listener.https.id
endpoint_group_region = "us-east-1"

# The "Exhaust Port" Health Check
health_check_port = 443
health_check_protocol = "HTTPS"
health_check_path = "/healthz"
health_check_interval_seconds = 10
threshold_count = 3 # Failover after 30 seconds of instability

endpoint_configuration {
endpoint_id = var.east_alb_arn
weight = 100
}
}

# Endpoint Group for US-WEST-2
resource "aws_globalaccelerator_endpoint_group" "west" {
listener_arn = aws_globalaccelerator_listener.https.id
endpoint_group_region = "us-west-2"

endpoint_configuration {
endpoint_id = var.west_alb_arn
weight = 100
}
}

module "eks" {
source = "terraform-aws-modules/eks/aws"
version = "~> 19.0"

cluster_name = "deathstar-${var.region}"
cluster_version = "1.28"
vpc_id = var.vpc_id
subnet_ids = var.private_subnets

# OIDC is the key for Zero Trust IAM (IRSA)
enable_irsa = true

eks_managed_node_groups = {
general = {
instance_types = ["t3.large"]
min_size = 3
max_size = 10
desired_size = 3
}
}
}

# IAM Role for the Superlaser Service (IRSA)
resource "aws_iam_role" "laser_role" {
name = "laser-executor-${var.region}"
assume_role_policy = data.aws_iam_policy_document.eks_oidc_assume_role.json
}

resource "aws_dynamodb_table" "targeting_data" {
name = "target-coordinates"
billing_mode = "PAY_PER_REQUEST"
hash_key = "PlanetId"
stream_enabled = true
stream_view_type = "NEW_AND_OLD_IMAGES"

attribute {
name = "PlanetId"
type = "S"
}

# This replicates data between regions in < 1 second
replica {
region_name = "us-east-1"
}
replica {
region_name = "us-west-2"
}
}