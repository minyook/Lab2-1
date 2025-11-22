use std::io;
use std::io::Write;

fn get_input(prompt: &str) -> i32 {
    print!("{}", prompt);
    io::stdout().flush().unwrap();

    let mut input_text = String::new();
    io::stdin()
        .read_line(&mut input_text)
        .expect("입력을 읽는데 실패했습니다.");

    match input_text.trim().parse() {
        Ok(num) => num,
        Err(_) => {
            println!("잘못된 입력입니다. 0으로 처리합니다.");
            0
        }
    }
}

fn main() {
    println!("=== 행렬 덧셈 프로그램 (Rust Vec) ===");

    let rows = get_input("행(Row)의 개수를 입력하세요: ") as usize;
    let cols = get_input("열(Col)의 개수를 입력하세요: ") as usize;

    println!("\n{} x {} 크기의 행렬을 생성합니다.\n", rows, cols);

    let mut matrix_a: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut matrix_b: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut matrix_sum: Vec<Vec<i32>> = vec![vec![0; cols]; rows];

    println!("--- 행렬 A의 내용을 입력하세요 ---");
    for i in 0..rows {
        for j in 0..cols {
            let prompt = format!("A[{}][{}] 값: ", i, j);
            matrix_a[i][j] = get_input(&prompt);
        }
    }

    println!("\n--- 행렬 B의 내용을 입력하세요 ---");
    for i in 0..rows {
        for j in 0..cols {
            let prompt = format!("B[{}][{}] 값: ", i, j);
            matrix_b[i][j] = get_input(&prompt);
        }
    }

    for i in 0..rows {
        for j in 0..cols {
            matrix_sum[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }

    println!("\n=== 결과 (A + B) ===");
    for i in 0..rows {
        for j in 0..cols {
            print!("{}\t", matrix_sum[i][j]);
        }
        println!();
    }
}
