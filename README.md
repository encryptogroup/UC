# Universal Circuit (UC) Compiler

### Implementation of Valiant's Universal Circuit Construction Optimized for Private Function Evaluation

By *Ágnes Kiss and Thomas Schneider* ([ENCRYPTO](http://www.encrypto.de), TU Darmstadt)<br>in [Eurocrypt 2016](http://ist.ac.at/eurocrypt2016/). Paper available [here](http://encrypto.de/papers/KS16.pdf).

### Features
---

Our Universal Circuit Compiler implements the most efficient UC construction, originally proposed by Leslie G. Valiant in STOC'76. 
It accepts any Boolean circuit as input in SHDL format, provided that the gates have at most two incoming edges, and outputs the topology of the UC along with its programming bits corresponding to the circuit.

This code is provided as a experimental implementation for testing purposes and should not be used in a productive environment. We cannot guarantee security and correctness.

### Requirements
---

Our UC compiler was developed in C/C++ and therefore requires [`g++`](https://packages.debian.org/testing/g++) using Linux.

### UC Compiler Sourcecode
---

#### File System Structure

* `/graphviz/`    - Graphviz files if graph debugging is enabled.
* `/circuits/`    - Example circuit files.
* `/src/`    - Source code.
 * `src/gamma/` - Source of underlying graph of the circuit.
 * `src/uc/` - Source of universal graph, its embedding and printing as universal circuit into file.
 * `src/util/` - Source of functions that are used for debugging or reading from the SHDL file.


#### UC Compiler

1. Clone a copy of the main UC git repository and its submodules by running:
	```
	git clone --recursive git://github.com/encryptogroup/UC
	```

2. Enter the Framework directory: `cd UC/src/`


### Examples
---

#### Included Example Circuits

  * Our UC compiler is compatible with SHDL format with gates with at most two inputs, i.e., any function generated with the [FairplayPF](http://thomaschneider.de/FairplayPF/) circuit compiler. Examples such as MobileCode and CreditChecking can be found under `/circuits/`.
  * Our UC compiler is also compatible with the test circuits for basic function from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), having to use an additional conversion function that translates these to SHDL circuits.
  * Further example applications will be added soon.

#### Testing Circuits

  * If you are using the format of the circuits from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), you add your circuit under `/circuits/` and set the name of your circuit, e.g., circuit_name, within bristol_to_SHDL.cpp and run:
```
g++ -o Bristol bristol_to_SHDL.cpp
./Bristol
```
  * If you generated your circuit file using FairplayPF or you are done with the previous step, next the fanout-2 gates are eliminated and the UC and its programming are generated. For generating and testing the UC, set the name of your circuit in UC.cpp and run:
```
g++ -o UC UC.cpp
./UC
```
  * On successful run, two files are generated: e.g., circuit_name_circ.txt with the UC topology and circuit_name_prog.txt with its programming corresponding to the input circuit.
  * You should get some debug output for you to verify the correctness of the computation.

#### Private Function Evaluation

  * Our UC compiler is compatible with ABY that can evaluate the UC generated. The code for this will be available soon at the github page of [ABY](https://github.com/encryptogroup/ABY).
  
