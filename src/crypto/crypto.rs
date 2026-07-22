use argon2::Argon2;
use crate::models::crypto_info::CryptoInfo;
struct Crypto{
    argon2: Argon2<'static>,
    crypto_info : CryptoInfo
}

impl Crypto {
    pub fn new() -> Crypto {
        Crypto {argon2: Argon2::default(), crypto_info : CryptoInfo::default()}
    }
}