### Репорт 

#### Програмна реалізація
##### Імплементація шифру "Струмок" складається з трьох файлів. 
- `strumok.h` (містить клас `StrumokCipher`)
- `strumok.cpp`  
- `strumok_tables.h` (містить матриці нелінійної підстановки T0 - T7, alpha_mul, alpha_inv)
- `strumok_tables.cpp`

<details>

<summary>Strumok UML diagram</summary>

[Strumok diagram](./STRUMOK/analysis/strumok_uml.png)

<details>

<summary>Mermaid chart code</summary>

```
classDiagram
    class StrumokCipher {
        +StrumokCipher(Mode mode)
        +void Init(vector<uint64_t> key, vector<uint64_t> IV)
        +void Next(NextMode nextMode = NextMode::kNormal) noexcept
        +uint64_t Strm() const noexcept
        -shared_ptr<IternalState> curState
        -Mode curMode
        -bool ifInitialized
        -static inline uint64_t kIvLength
        -static inline uint64_t FSM(uint64_t x, uint64_t y, uint64_t z)
        -static inline uint64_t a_mul(uint64_t x)
        -static inline uint64_t ainv_mul(uint64_t x)
        -static inline uint64_t transform_T(uint64_t x)
    }

    class Mode {
        <<enumeration>>
        +k256Bit = 4
        +k512Bit = 8
    }

    class NextMode {
        <<enumeration>>
        +kInit = 0
        +kNormal = 1
    }

    class IternalState {
        +IternalState()
        +IternalState(const IternalState& other)
        +vector<uint64_t> linearRegisters
        +uint64_t r1
        +uint64_t r2
    }

    %% Relationships
    StrumokCipher "1" --> "1" IternalState
    StrumokCipher "1" --> "1" Mode
    StrumokCipher "1" --> "1" NextMode

```

</details>

</details>

#### AutoGuess analysis 
