use std::collections::HashMap;
use std::io;
use std::io::Write;

fn get_input(prompt: &str) -> String {
    print!("{}", prompt);
    io::stdout().flush().unwrap(); // 프롬프트 즉시 출력

    let mut input_text = String::new();
    io::stdin()
        .read_line(&mut input_text)
        .expect("입력을 읽는데 실패했습니다.");

    // 입력 끝의 줄바꿈(\n) 문자를 제거하여 반환
    input_text.trim().to_string()
}

fn main() {
    // 해시맵 생성 (Key: 이름, Value: 전화번호)
    let mut phonebook: HashMap<String, String> = HashMap::new();

    println!("=== Rust 전화번호부 프로그램 ===");

    loop {
        println!("\n[메뉴 선택]");
        println!("1. 전화번호 추가");
        println!("2. 전화번호 검색");
        println!("3. 전체 목록 보기");
        println!("4. 종료");
        
        let choice = get_input("선택> ");

        match choice.as_str() {
            "1" => {
                let name = get_input("이름 입력: ");
                let number = get_input("전화번호 입력: ");
                phonebook.insert(name, number);
                println!("-> 저장되었습니다.");
            }
            "2" => {
                let name = get_input("검색할 이름: ");
                match phonebook.get(&name) {
                    Some(number) => println!("-> {}의 번호: {}", name, number),
                    None => println!("-> '{}' 님을 찾을 수 없습니다.", name),
                }
            }
            "3" => {
                println!("--- 저장된 목록 ---");
                if phonebook.is_empty() {
                    println!("(비어있음)");
                } else {
                    for (name, number) in &phonebook {
                        println!("이름: {}, 번호: {}", name, number);
                    }
                }
            }
            "4" => {
                println!("프로그램을 종료합니다.");
                break;
            }
            _ => println!("잘못된 입력입니다. 다시 선택해주세요."),
        }
    }
}
