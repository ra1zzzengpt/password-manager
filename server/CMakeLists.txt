add_executable(server main.cpp)

target_compile_features(server PRIVATE cxx_std_23)

# OpenSSL libpqxx boost