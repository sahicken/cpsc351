use std::fs::File;
use std::io::{self, BufRead};
use std::sync::{Arc, Mutex};
use std::thread;
use rand::Rng;

#[derive(Debug)]
struct System {
    available: Vec<i32>,
    maximum: Vec<Vec<i32>>,
    allocation: Vec<Vec<i32>>,
    need: Vec<Vec<i32>>,
    num_processes: usize,
    num_resources: usize,
}

impl System {
    fn new(available: Vec<i32>, maximum: Vec<Vec<i32>>, allocation: Vec<Vec<i32>>) -> Self {
        let num_processes = maximum.len();
        let num_resources = available.len();
        let need = maximum
            .iter()
            .zip(allocation.iter())
            .map(|(max, alloc)| {
                max.iter()
                    .zip(alloc.iter())
                    .map(|(m, a)| m - a)
                    .collect()
            })
        .collect();

        System {
            available,
            maximum,
            allocation,
            need,
            num_processes,
            num_resources,
        }
    }

    fn is_safe(&self) -> bool {
        let mut work = self.available.clone();
        let mut finish = vec![false; self.num_processes];
        let mut safe_sequence = Vec::new();

        loop {
            let mut found = false;
            for i in 0..self.num_processes {
                if !finish[i] && self.need[i].iter().zip(&work).all(|(n, w)| n <= w) {
                    for j in 0..self.num_resources {
                        work[j] += self.allocation[i][j];
                    }
                    finish[i] = true;
                    safe_sequence.push(i);
                    found = true;
                }
            }
            if !found {
                break;
            }
        }

        if finish.iter().all(|&f| f) {
            println!("System is in a safe state: {:?}", safe_sequence);
            true
        } else {
            println!("System is in an unsafe state.");
            false
        }
    }

    fn request_resources(&mut self, process_id: usize, request: Vec<i32>) -> bool {
        if request.iter().zip(&self.need[process_id]).any(|(r, n)| r > n) {
            println!("Process {} has made an invalid request.", process_id);
            return false;
        }

        if request.iter().zip(&self.available).any(|(r, a)| r > a) {
            println!("Process {} has to wait; not enough resources available.", process_id);
            return false;
        }

        // Pretend to allocate resources
        for j in 0..self.num_resources {
            self.available[j] -= request[j];
            self.allocation[process_id][j] += request[j];
            self.need[process_id][j] -= request[j];
        }

        if self.is_safe() {
            println!("Resources granted to Process {}: {:?}", process_id, request);
            true
        } else {
            // Rollback
            for j in 0..self.num_resources {
                self.available[j] += request[j];
                self.allocation[process_id][j] -= request[j];
                self.need[process_id][j] += request[j];
            }
            println!("Request denied for Process {}: {:?}", process_id, request);
            false
        }
    }

    fn release_resources(&mut self, process_id: usize) {
        for j in 0..self.num_resources {
            self.available[j] += self.allocation[process_id][j];
            self.allocation[process_id][j] = 0;
            self.need[process_id][j] = self.maximum[process_id][j];
        }
        println!("Process {} has released its resources.", process_id);
    }
}

fn main() -> io::Result<()> {
    let file = File::open("input.txt")?;
    let reader = io::BufReader::new(file);
    let mut lines = reader.lines();

    // Read number of resources and processes
    let num_resources: usize = lines.next().ok_or_else(|| {
        io::Error::new(io::ErrorKind::InvalidData, "Missing number of resources")
    })?.unwrap().parse().map_err(|_| {
        io::Error::new(io::ErrorKind::InvalidData, "Invalid number of resources")
    })?;

    let num_processes: usize = lines.next().ok_or_else(|| {
        io::Error::new(io::ErrorKind::InvalidData, "Missing number of processes")
    })?.unwrap().parse().map_err(|_| {
        io::Error::new(io::ErrorKind::InvalidData, "Invalid number of processes")
    })?;  

    // Read available resources
    let available: Vec<i32> = lines.next().ok_or_else(|| {
        io::Error::new(io::ErrorKind::InvalidData, "Missing available resources")
    })?.unwrap()
    .split_whitespace()
        .map(|s| s.parse().map_err(|_| {
            io::Error::new(io::ErrorKind::InvalidData, "Invalid available resource value")
        })).collect::<Result<Vec<i32>, _>>()?;

    // Read maximum resource requirements for each process
    let mut maximum: Vec<Vec<i32>> = Vec::new();
    for _ in 0..num_processes {
        let line = lines.next().ok_or_else(|| {
            io::Error::new(io::ErrorKind::InvalidData, "Missing maximum resources for a process")
        })?.unwrap();
        let process_max: Vec<i32> = line.split('|')
            .map(|s| {
                s.split_whitespace()
                    .map(|n| n.parse().map_err(|_| {
                        io::Error::new(io::ErrorKind::InvalidData, "Invalid maximum resource value")
                    })).collect::<Result<Vec<i32>, _>>()
            })
        .collect::<Result<Vec<Vec<i32>>, _>>()?
            .into_iter()
            .flat_map(|v| v) // Flatten the Vec<Vec<i32>> into Vec<i32>
            .collect();
        maximum.push(process_max);
    }

    // Read current allocation for each process
    let mut allocation: Vec<Vec<i32>> = Vec::new();
    for _ in 0..num_processes {
        let line = lines.next().ok_or_else(|| {
            io::Error::new(io::ErrorKind::InvalidData, "Missing allocation for a process")
        })?.unwrap();

        let process_alloc: Vec<i32> = line.split('|')
            .map(|s| {
                s.split_whitespace()
                    .map(|n| n.parse().map_err(|_| {
                        io::Error::new(io::ErrorKind::InvalidData, "Invalid allocation value")
                    })).collect::<Result<Vec<i32>, _>>()
            })
        .collect::<Result<Vec<Vec<i32>>, _>>()?
            .into_iter()
            .flat_map(|v| v) // Flatten the Vec<Vec<i32>> into Vec<i32>
            .collect();
        allocation.push(process_alloc);
    }

    // Initialize the system
    let system = System::new(available, maximum, allocation);
    let system_clone = Arc::new(Mutex::new(system)); // Wrap the system in Arc<Mutex<>> once

    // Create a vector to hold thread handles
    let mut handles = vec![];

    // Simulate each process in a separate thread
    for process_id in 0..num_processes {
        let system_clone = Arc::clone(&system_clone); // Clone the Arc for each thread
        let handle = thread::spawn(move || {
            let mut rng = rand::thread_rng();
            // Simulate random resource requests
            for _ in 0..5 { // Each process makes 5 requests
                let request: Vec<i32> = (0..num_resources)
                    .map(|_| rng.gen_range(0..=5)) // Random request between 0 and 5
                    .collect();

                let mut system = system_clone.lock().unwrap();
                system.request_resources(process_id, request);
            }
            // Release resources after finishing
            let mut system = system_clone.lock().unwrap();
            system.release_resources(process_id);
        });
        handles.push(handle);
    }

    // Wait for all threads to finish
    for handle in handles {
        handle.join().unwrap();
    }

    println!("All processes have completed.");
    Ok(())
}        
