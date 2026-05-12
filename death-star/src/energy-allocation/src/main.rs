use std::collections::HashMap;

#[derive(Clone, Debug, PartialEq)]
pub struct Subsystem {
    pub name: String,
    pub priority: u32,
    pub requested: f64,
    pub guaranteed: f64,
}

pub fn allocate_energy(
    total_energy: f64,
    systems: &[Subsystem],
) -> HashMap<String, f64> {
    let mut allocations = HashMap::new();

    let guaranteed_sum: f64 = systems.iter().map(|s| s.guaranteed).sum();

    // Degradation trigger: if guarantees exceed total energy, clamp to total energy.
    if guaranteed_sum > total_energy {
        // Distribute proportionally based on guaranteed minimums if we can't meet them
        let scale = if guaranteed_sum > 0.0 { total_energy / guaranteed_sum } else { 0.0 };
        return systems
            .iter()
            .map(|s| (s.name.clone(), s.guaranteed * scale))
            .collect();
    }

    let remaining = total_energy - guaranteed_sum;
    let total_priority: u32 = systems.iter().map(|s| s.priority).sum();

    for s in systems {
        let mut allocation = s.guaranteed;

        if total_priority > 0 {
            let weighted = (s.priority as f64 / total_priority as f64) * remaining;

            // Only allocate up to what was requested, considering the guarantee
            let additional_needed = if s.requested > s.guaranteed {
                s.requested - s.guaranteed
            } else {
                0.0
            };

            allocation += weighted.min(additional_needed);
        }

        allocations.insert(s.name.clone(), allocation);
    }

    allocations
}

fn main() {
    println!("Energy Allocation Service starting...");

    let systems = vec![
        Subsystem {
            name: "Weapon".to_string(),
            priority: 70,
            requested: 70.0,
            guaranteed: 0.0,
        },
        Subsystem {
            name: "Shields".to_string(),
            priority: 40,
            requested: 40.0,
            guaranteed: 10.0,
        },
        Subsystem {
            name: "Life Support".to_string(),
            priority: 10, // Priority still matters for surplus
            requested: 20.0,
            guaranteed: 10.0,
        },
    ];

    let total_energy = 100.0;
    let allocations = allocate_energy(total_energy, &systems);

    println!("Total Energy: {}", total_energy);
    for (name, allocated) in allocations {
        println!("{}: allocated {:.2}", name, allocated);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_normal_allocation() {
        let systems = vec![
            Subsystem { name: "Weapon".to_string(), priority: 70, requested: 70.0, guaranteed: 0.0 },
            Subsystem { name: "Shields".to_string(), priority: 40, requested: 40.0, guaranteed: 10.0 },
            Subsystem { name: "Life Support".to_string(), priority: 10, requested: 20.0, guaranteed: 10.0 },
        ];

        let allocs = allocate_energy(100.0, &systems);

        // Guarantees = 20.0. Remaining = 80.0
        // Priorities: W:70, S:40, L:10. Total Priority = 120
        // Weapon gets: 0 + (70/120 * 80) = 46.66...
        // Shields gets: 10 + min(40-10, (40/120 * 80)) = 10 + min(30, 26.66...) = 36.66...
        // Life Support gets: 10 + min(20-10, (10/120 * 80)) = 10 + min(10, 6.66...) = 16.66...

        assert!((allocs["Weapon"] - 46.66).abs() < 0.1);
        assert!((allocs["Shields"] - 36.66).abs() < 0.1);
        assert!((allocs["Life Support"] - 16.66).abs() < 0.1);
    }

    #[test]
    fn test_degradation_mode() {
        let systems = vec![
            Subsystem { name: "Weapon".to_string(), priority: 70, requested: 70.0, guaranteed: 0.0 },
            Subsystem { name: "Shields".to_string(), priority: 40, requested: 40.0, guaranteed: 20.0 },
            Subsystem { name: "Life Support".to_string(), priority: 10, requested: 20.0, guaranteed: 20.0 },
        ];

        // Total guaranteed is 40.0, but we only have 20.0 energy
        let allocs = allocate_energy(20.0, &systems);

        // Should scale proportionally: Shields gets 10, Life Support gets 10, Weapon gets 0
        assert_eq!(allocs["Weapon"], 0.0);
        assert_eq!(allocs["Shields"], 10.0);
        assert_eq!(allocs["Life Support"], 10.0);
    }
}
