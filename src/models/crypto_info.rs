
pub struct CryptoInfo {
    salt: Vec<u8>,
    key: Vec<u8>,
    ciphertext: Vec<u8>,
}

impl CryptoInfo {
    pub fn default() -> CryptoInfo {
        CryptoInfo {salt: Vec::new(), key: Vec::new(), ciphertext: Vec::new()}
    }
}