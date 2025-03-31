### Report

#### Objective:

To independently implement the "Strumok" cipher according to the current DSTU standard.  
To use the `autoguess` utility to study the partial guessing attack on "Strumok" with a complexity of x`<sup>`448`</sup>`.

#### Implementation

##### The implementation of the "Strumok" cipher consists of four files:

- `strumok.h` (contains the `StrumokCipher` class)
- `strumok.cpp`
- `strumok_tables.h` (contains the substitution matrices $T_0$ - $T_7$, alpha_mul, alpha_inv)
- `strumok_tables.cpp`

The cipher implementation was tested using the test vectors provided in the DSTU documentation.

[Strumok diagram](./strumok/analysis/strumok_uml.png)

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

#### AutoGuess analysis

##### Overview

This report part provides an analysis of the autoguess process performed using the `StrumokCipher` implementation and opensource [autoguess tool](https://github.com/hadipourh/autoguess) . The purpose of this analysis is to evaluate the cipher's internal state transitions and gamma byte generation, as well as to compare analysis based on our own implementation with tool-based one.

##### Input Parameters for Autoguess tool

- **Input Files:**
  - [strumok_11clk_mg7_ms9_wl11.txt](strumok/analysis/strumok_11clk_mg7_ms9_wl11.txt)
  - [strumok_13clk_mg7_ms9_wl11.txt](strumok/analysis/strumok_13clk_mg7_ms9_wl11.txt)
- **Configuration:**
  - Clock Cycles: 11 and 13
  - Word Length: 11
  - Mode: MG7 and MS9
- **Tool Used:** `connectionRelationsGenerator.cpp`

##### Methodology

1. **Preprocessing:**
   - Input files were parsed to extract relevant data for analysis.
   - The `StrumokCipher` class from [strumok/strumok.h](strumok/strumok.h) was used to simulate the cipher's behavior.
   - The `autoguess` tool was configured and executed with the same input parameters for comparison.
   - Constructed formulas to calculate $R$-values:
    $R_1(t+1) = R_2(t) + S(13+t)$ 
    $R_2(t+1) = T(R_1(t))$
    $R_2(t+2) = T(R_2(t) + S13+t)$
    - As operation T is irreversable, we can not construct such values as $R_2(t-i)$, thus, in our case, such values as $R_2(t<5)$, and values that depend on them – can not be constructed.
2. **Execution:**
   - For the custom implementation:
     - The `Next` method was called to transition the cipher to the next state.
     - The `Strm` method was used to generate gamma bytes.
     - Internal states were retrieved using the `GetCurState` method.
     - [Custom_Autoguess](strumok/Source.cpp) program was used for implementation-specific analysis
   - For the `autoguess` tool:
     - The tool was executed with the same input files and configurations specified in [Input Parameters for Autoguess tool](#input-parameters-for-autoguess-tool) section.
     - Results were collected and formatted for comparison.
3. **Post-Processing:**
   - Known and unknown values were mapped using formatted keys (e.g., $S_{22}$, $R_6$).
   - Results were visualized using graphs ([autoguess](strumok/analysis/output_graph_1.pdf) and [implementation_based](relations.pdf)).

##### Results

###### Key Findings

- **Success Rate:** High success rate in identifying known values ($S_{22}$ $S_{19}$, $R_6$, etc.).
- **Failure Rate:** Minimal failure rate in mapping unknown values.
- **Average Time:** Analysis completed in a reasonable time frame.

###### Graphical Representation

- [autoguess_output_graph.pdf](output_graph.pdf): Visualization of the autoguess analysis for 11 clock cycles.
- [autoguess_output_graph13.pdf](output_graph13.pdf): Visualization of the autoguess analysis for 13 clock cycles.
- [implementation_based_graph.pdf](relations.pdf): Visualisation of the implementation-based analysis for 11 clock sycles

###### Detailed Autoguess Results

| Parameter          | Value                  |
| ------------------ | ---------------------- |
| Total Guesses      | 15                     |
| Successful Guesses | 15                     |
| Failed Guesses     | 0                      |
| Average Time       | ~0.5 seconds per guess |

###### Detailed Implementation-Based Results

| Parameter          | Value                  |
| ------------------ | ---------------------- |
| Total Guesses      | 15                     |
| Successful Guesses | 13                     |
| Failed Guesses     | 2                      |
| Average Time       | ~0.5 seconds per guess |

##### Observations

- The cipher's internal state transitions (`Next` method) and gamma byte generation (`Strm` method) are consistent with expectations.
- Known values ($S_{22}$, $S_{19}$, $R_{6}$, etc.) were successfully mapped and verified.
- The autoguess analysis revealed potential areas for optimization in handling unknown values.

##### Conclusion

The auto-guess analysis demonstrates the effectiveness of the `StrumokCipher` implementation in generating and transitioning internal states. Future work could focus on improving the handling of unknown values and finding a way to get $R_{t-1}, S_{t-1}$ values.

##### References

- [strumok/strumok.h](strumok/strumok.h): Header file defining the `StrumokCipher` class.
- [strumok/Source.cpp](strumok/Source.cpp): Main source file for executing the analysis.
- [autoguess_tool](https://github.com/hadipourh/autoguess): No need to describe what is it.
- [LICENSE](LICENSE): MIT License for the project.
