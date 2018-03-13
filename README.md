# Universal Circuit (UC) Compiler

### Implementation of Valiant's Universal Circuit Constructions

By *Ágnes Kiss and Thomas Schneider* ([ENCRYPTO](http://www.encrypto.de), TU Darmstadt) in [Eurocrypt 2016](http://ist.ac.at/eurocrypt2016/). Paper available [here](http://encrypto.de/papers/KS16.pdf), and <br>
by *Daniel Günther, Ágnes Kiss and Thomas Schneider* in [Asiacrypt 2017](https://asiacrypt.iacr.org/2017/). Paper available [here](http://encrypto.de/papers/GKS17.pdf).

### Features
---

Our Universal Circuit Compiler implements the most efficient UC constructions, originally proposed by Leslie G. Valiant in STOC'76, namely the 2-way and 4-way split UC constructions, that are based on a 2-way or 4-way recursive structure. 
It accepts any Boolean circuit as input in SHDL format, provided that the gates have at most two incoming edges, and outputs the topology of the UC along with its programming bits corresponding to the circuit.

This code is provided as a experimental implementation for testing purposes and should not be used in a productive environment. We cannot guarantee security and correctness.

### Requirements
---

* A **Linux distribution** of your choice (The UC compiler was tested with [Ubuntu](https://ubuntu.com) and [Kali](https://kali.org))
* The following packages:
 * **g++**
 * **make**
* Optional: **[CMake](https://cmake.org)** to compile the code with an alternative compiler (e.g. if the make file does not work for you or if you want to compile it with Windows)

### UC Compiler Sourcecode
---

#### File System Structure

* `/circuits/`    - Example circuit files.
* `/graphviz/`    - Graphviz files if graph debugging is enabled.
* `/output/circuits/`    - Output circuit files.
* `/src/`    - Source code.
 * `src/Gamma/` - Source of underlying graph of the circuit.
 * `src/uc/` - Source of universal graph, its embedding and printing as universal circuit into file.
  * `src/uc/2way` - Source of the 2-way split UC construction.
  * `src/uc/4way` - Source of the 4-way split UC construction.
  
#### UC Compiler
---

1. Clone a copy of the main UC git repository and its submodules by running:
```
git clone --recursive git://github.com/encryptogroup/UC
```
2.  Enter the UC directory: `cd UC`

##### Compiling with make
1.  Call `make` in the root directory to compile the whole code and generate the two executables **UC** and **bristol**

##### Compiling with cmake
1. Choose one of the compilers [here](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) (e.g. *CodeBlocks - MinGW*) and run the following commands:
```
cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - MinGW"
cmake --build ./ --target all -- -j 2
```

### Examples
---

#### Included Example Circuits

  * Our UC compiler is compatible with SHDL format with gates with at most two inputs, i.e., any function generated with the [FairplayPF](http://thomaschneider.de/FairplayPF/) circuit compiler. Examples such as MobileCode and CreditChecking can be found under `/circuits/`.
  * Our UC compiler is also compatible with the test circuits for basic function from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), having to use an additional conversion function that translates these to SHDL circuits.
  * Further example applications will be added soon.

#### Testing Circuits

  * If you are using the format of the circuits from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), you add your circuit under `/circuits/` (e.g. *adder_32bit.txt*) and set the name of your circuit, e.g., circuit_name, within bristol_to_SHDL.cpp and run:
```
./Bristol adder32_bit.txt
```
  * If you generated your circuit file using FairplayPF or you are done with the previous step, next the fanout-2 gates are eliminated and the UC and its programming are generated. For generating and testing the UC, run:
```
./UC adder_32bit.txt_SHDL.circuit
```
  * On successful run, two files are generated in the folder output/circuits: e.g., circuit_name_circ.txt with the UC topology and circuit_name_prog.txt with its programming corresponding to the input circuit.
  * You should get some debug output for you to verify the correctness of the computation.
   
##### UC Options
* You can add the -version flag with values 2 or 4 to use the 2-way split construction or the 4-way split construction (as default we use the 4-way split construction if the flag isn't set):
```
./UC -version 2 adder_32bit.txt_SHDL.circuit
```

* There can be some flags set or removed in src/config.h:
 * **TIME** - measures the time of the UC generation
 * **STATE** - outputs state information while generating the UC
 * **DEBUG_EMBEDDING** - outputs some information that is helpful for debugging the edge-embedding process
 * **DEBUG_GRAPH** - outputs graphiviz files in the graphviz folder
 * **DEBUG_CORRECTNESS** - generates a random input and evaluates the original circuit and the UC with it. Afterwards, the results are compared.


#### Private Function Evaluation

  * Our UC compiler is compatible with ABY that can evaluate the UC generated. The code for this will be available soon at the github page of [ABY](https://github.com/encryptogroup/ABY).
  
