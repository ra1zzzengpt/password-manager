use rand::Rng;
const ALLOWED_SYMBOLS : &[u8] = b"abcdefghijklmnopqrstuvwxyz\
                             ABCDEFGHIJKLMNOPQRSTUVWXYZ\
                             0123456789!@#$%^&*()";

pub fn generate_password(length : usize) -> String {
    let mut password : String = "".to_string();
    let mut rng = rand::thread_rng();
    for _ in 0 .. length {
        password.push(ALLOWED_SYMBOLS[rng.gen_range(0..ALLOWED_SYMBOLS.len())] as char)
    }
    password
}