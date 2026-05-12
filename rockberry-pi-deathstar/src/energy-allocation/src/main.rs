use std::collections::HashMap;
use fixed::types::extra::U12;
use fixed::FixedU32;
use tokio::sync::{mpsc, oneshot};

// 20 integer bits, 12 fractional bits
pub type Energy = FixedU32<U12>;

#[derive(Clone, Debug, PartialEq)]
pub struct SubsystemFixed {
    pub name: String,
    pub priority: u32,
    pub requested: Energy,
    pub guaranteed: Energy,
}

#[derive(Clone, Debug)]
pub struct SubsystemTelemetry {
    pub name: String,
    pub priority: u32,
    pub requested: f64,
    pub guaranteed: f64,
}

// Message types for actor communication
#[derive(Debug)]
pub enum AllocatorMsg {
    Telemetry(Vec<SubsystemTelemetry>),
    GetAllocations(oneshot::Sender<HashMap<String, f64>>),
}

pub struct AllocatorActor {
    receiver: mpsc::Receiver<AllocatorMsg>,
    current_alloc: HashMap<String, Energy>,
    total_energy: Energy,
}

impl AllocatorActor {
    pub fn new(receiver: mpsc::Receiver<AllocatorMsg>, total_energy_f64: f64) -> Self {
        Self {
            receiver,
            current_alloc: HashMap::new(),
            total_energy: Energy::from_num(total_energy_f64),
        }
    }

    pub async fn run(mut self) {
        while let Some(msg) = self.receiver.recv().await {
            match msg {
                AllocatorMsg::Telemetry(telemetry) => {
                    let systems: Vec<SubsystemFixed> = telemetry.into_iter().map(|t| {
                        SubsystemFixed {
                            name: t.name,
                            priority: t.priority,
                            requested: Energy::from_num(t.requested),
                            guaranteed: Energy::from_num(t.guaranteed),
                        }
                    }).collect();
                    self.current_alloc = allocate_energy_fixed(self.total_energy, &systems);
                }
                AllocatorMsg::GetAllocations(tx) => {
                    let float_allocs = self.current_alloc.iter()
                        .map(|(k, v)| (k.clone(), v.to_num::<f64>()))
                        .collect();
                    let _ = tx.send(float_allocs);
                }
            }
        }
    }
}

pub fn allocate_energy_fixed(
    total_energy: Energy,
    systems: &[SubsystemFixed],
) -> HashMap<String, Energy> {
    let mut allocations = HashMap::new();

    let guaranteed_sum: Energy = systems.iter().map(|s| s.guaranteed).sum();

    // Degradation trigger
    if guaranteed_sum > total_energy {
        // We use integer arithmetic or float-conversion for scaling to avoid fixed-point division limits
        // scale = total / guaranteed_sum
        let scale_f: f64 = total_energy.to_num::<f64>() / guaranteed_sum.to_num::<f64>();
        let scale = Energy::from_num(scale_f);

        return systems
            .iter()
            .map(|s| {
                // Approximate scaling
                let scaled_f = s.guaranteed.to_num::<f64>() * scale.to_num::<f64>();
                (s.name.clone(), Energy::from_num(scaled_f))
            })
            .collect();
    }

    let remaining = total_energy - guaranteed_sum;
    let total_priority: u32 = systems.iter().map(|s| s.priority).sum();

    for s in systems {
        let mut allocation = s.guaranteed;

        if total_priority > 0 {
            // (s.priority / total_priority) * remaining
            let weight_f = (s.priority as f64) / (total_priority as f64);
            let weighted_f = weight_f * remaining.to_num::<f64>();
            let weighted = Energy::from_num(weighted_f);

            let additional_needed = if s.requested > s.guaranteed {
                s.requested - s.guaranteed
            } else {
                Energy::from_num(0)
            };

            allocation += weighted.min(additional_needed);
        }

        allocations.insert(s.name.clone(), allocation);
    }

    allocations
}

#[tokio::main]
async fn main() {
    println!("Hardened Energy Allocation Service (Fixed Point / Actor Model) starting...");

    let (tx, rx) = mpsc::channel(100);  // Bounded for backpressure
    let actor = AllocatorActor::new(rx, 100.0);

    // Spawn the actor task
    tokio::spawn(actor.run());

    // Send some telemetry
    let telemetry = vec![
        SubsystemTelemetry {
            name: "Weapon".to_string(),
            priority: 70,
            requested: 70.0,
            guaranteed: 0.0,
        },
        SubsystemTelemetry {
            name: "Shields".to_string(),
            priority: 40,
            requested: 40.0,
            guaranteed: 10.0,
        },
        SubsystemTelemetry {
            name: "Life Support".to_string(),
            priority: 10,
            requested: 20.0,
            guaranteed: 10.0,
        },
    ];

    let _ = tx.send(AllocatorMsg::Telemetry(telemetry)).await;

    // Request the current allocations
    let (resp_tx, resp_rx) = oneshot::channel();
    let _ = tx.send(AllocatorMsg::GetAllocations(resp_tx)).await;

    if let Ok(allocs) = resp_rx.await {
        println!("Received Allocations from Actor:");
        for (name, allocated) in allocs {
            println!("{}: allocated {:.2}", name, allocated);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_normal_allocation_fixed() {
        let systems = vec![
            SubsystemFixed { name: "Weapon".to_string(), priority: 70, requested: Energy::from_num(70.0), guaranteed: Energy::from_num(0.0) },
            SubsystemFixed { name: "Shields".to_string(), priority: 40, requested: Energy::from_num(40.0), guaranteed: Energy::from_num(10.0) },
            SubsystemFixed { name: "Life Support".to_string(), priority: 10, requested: Energy::from_num(20.0), guaranteed: Energy::from_num(10.0) },
        ];

        let allocs = allocate_energy_fixed(Energy::from_num(100.0), &systems);

        let weapon_alloc = allocs["Weapon"].to_num::<f64>();
        let shields_alloc = allocs["Shields"].to_num::<f64>();
        let life_support_alloc = allocs["Life Support"].to_num::<f64>();

        assert!((weapon_alloc - 46.66).abs() < 0.1);
        assert!((shields_alloc - 36.66).abs() < 0.1);
        assert!((life_support_alloc - 16.66).abs() < 0.1);
    }

    #[test]
    fn test_degradation_mode_fixed() {
        let systems = vec![
            SubsystemFixed { name: "Weapon".to_string(), priority: 70, requested: Energy::from_num(70.0), guaranteed: Energy::from_num(0.0) },
            SubsystemFixed { name: "Shields".to_string(), priority: 40, requested: Energy::from_num(40.0), guaranteed: Energy::from_num(20.0) },
            SubsystemFixed { name: "Life Support".to_string(), priority: 10, requested: Energy::from_num(20.0), guaranteed: Energy::from_num(20.0) },
        ];

        let allocs = allocate_energy_fixed(Energy::from_num(20.0), &systems);

        let weapon_alloc = allocs["Weapon"].to_num::<f64>();
        let shields_alloc = allocs["Shields"].to_num::<f64>();
        let life_support_alloc = allocs["Life Support"].to_num::<f64>();

        assert_eq!(weapon_alloc, 0.0);
        assert_eq!(shields_alloc, 10.0);
        assert_eq!(life_support_alloc, 10.0);
    }
}
