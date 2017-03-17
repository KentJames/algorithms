// Factorises numbers and finds all their prime factors!

use std::io;

fn main(){

    let mut prime_number = String::new();
    let mut prime_factors: Vec<u64> = Vec::new(); 
    println!("Please enter your number you want to prime factor:");
    io::stdin().read_line(&mut prime_number)
        .expect("Failed to read line.");
     
    
    let mut prime_number: u64 = prime_number.trim().parse()
        .expect("Not a number!");
    println!("Prime number to factorize: {}",prime_number); 

    let original_prime: u64 = prime_number;

    'outer: loop{
        'inner: for i in (2..5000000000).filter(|&x| x % 2 > 0) {
            //println!("Iteration: {}",i);
            //println!("Number: {}",prime_number);
            if prime_number%i == 0 {
                println!("Prime factor found?: {}",i);
                prime_number = prime_number/i;
                prime_factors.push(i as u64);
                break 'inner;
            }
            else {
                continue;
            }

        }
        let mut check_var: u64 = 1;
        'check: for i in &prime_factors {
            check_var = check_var * i;

            if check_var == original_prime {
                break 'outer;
            }
            
            
        }

    }

}

