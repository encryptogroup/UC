
# Universal Circuit (UC) Compiler

### Implementation of Valiant's Universal Circuit Constructions

By *Ágnes Kiss and Thomas Schneider* ([ENCRYPTO](http://www.encrypto.de), TU Darmstadt) in [Eurocrypt 2016](http://ist.ac.at/eurocrypt2016/). Paper available [here](http://encrypto.de/papers/KS16.pdf), and <br>
by *Daniel Günther, Ágnes Kiss and Thomas Schneider* in [Asiacrypt 2017](https://asiacrypt.iacr.org/2017/). Paper available [here](http://encrypto.de/papers/GKS17.pdf), and <br>
by *Masaud Y. Alhassan, Daniel Günther, Ágnes Kiss and Thomas Schneider* (in submission). ePrint Paper available [here](https://eprint.iacr.org/2019/348.pdf) 


### Features
---

Our Universal Circuit Compiler implements the most efficient UC constructions, originally proposed by Leslie G. Valiant in STOC'76, namely the 2-way and 4-way split UC constructions, that are based on a 2-way or 4-way recursive structure. We provide implementations for the original 4-way split UC by Valiant as well as the improved 4-way split UC by Zhao et al. (ePrint Paper available [here](https://eprint.iacr.org/2018/943.pdf)). Additionally we implement our hybrid UC construction that combines Valiant's 2-way and one of the two 4-way split constructions so that the smallest UC for each circuit size is achieved.
Our UC compiler accepts any Boolean circuit as input in SHDL format, provided that the gates have at most two incoming edges, and outputs the topology of the UC along with its programming bits corresponding to the circuit.

This code is provided as a experimental implementation for testing purposes and should not be used in a productive environment. We cannot guarantee security and correctness.

### Requirements
---

* A **Linux distribution** of your choice (The UC compiler was tested with [Ubuntu](https://ubuntu.com) and [Kali](https://kali.org))
* The following packages:
 * **g++**
 * **make**
* **[CMake](https://cmake.org)**
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
3. Run the following commands to compile the code with CMake:
```
mkdir build && cd build
cmake ..
make
```
Hint: Unfortunately we have a bug with CMake so that our UC compiler runs only with CMake Debug mode. We are actually working on this issue.

### Examples
---

#### Included Example Circuits

  * Our UC compiler is compatible with SHDL format with gates with at most two inputs, i.e., any function generated with the [FairplayPF](http://thomaschneider.de/FairplayPF/) circuit compiler. Examples such as MobileCode and CreditChecking can be found under `/circuits/`.
  * Our UC compiler is also compatible with the test circuits for basic function from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), having to use an additional conversion function that translates these to SHDL circuits.
  * Further example applications will be added soon.

#### Testing Circuits

  * If you are using the format of the circuits from [here](https://www.cs.bris.ac.uk/Research/CryptographySecurity/MPC/), you add your circuit under `/circuits/` (e.g. *adder_32bit.txt*) and set the name of your circuit, e.g., circuit_name, within bristol_to_SHDL.cpp and run:
```
./bristol adder_32bit.txt
```
  * If you generated your circuit file using FairplayPF or you are done with the previous step, next the fanout-2 gates are eliminated and the UC and its programming are generated. For generating and testing the UC, run:
```
./UC adder_32bit.txt_SHDL.circuit
```
  * On successful run, two files are generated in the folder output/circuits: e.g., circuit_name_circ.txt with the UC topology and circuit_name_prog.txt with its programming corresponding to the input circuit.
  * You should get some debug output for you to verify the correctness of the computation.
   
##### UC Options
* If you run the UC as described above you will reach the best size that our implementation can achieve. Concretely, the UC is built with our hybrid construction using the 4-way Blocks of Zhao et al. <b>Other options should only be used for experiments!</b>
* You can add the -version flag with values 0 (for hybrid), 2 or 4 to use the 2-way split construction or the 4-way split construction (as default we use the hybrid if the flag isn't set):
```
./UC -version 2 adder_32bit.txt_SHDL.circuit
```
* You can add the -valiant flag if you want to use the original 4-way block of Valiant (in combination with the hybrid or the 4-way version)
* You can add the -random flag with a numeric value <i>n</i> if you want to run a UC on random circuit of size <i>n</i>.

* There can be some flags set or removed in src/config.h:
 * **TIME** - measures the time of the UC generation
 * **STATE** - outputs state information while generating the UC
 * **CIRCUIT_DEBUG** - outputs some information that is helpful for debugging the edge-embedding process
 * **DEBUG_GRAPH** - outputs graphiviz files in the graphviz folder
 * **DEBUG_CORRECTNESS** - generates a random input and evaluates the original circuit and the UC with it. Afterwards, the results are compared.

### Output

* The output of our UC compiler is composed of two files: a UC file appended with *_circ.txt* and a programming file with *_prog.txt*. These files contain the universal circuit and its programming, respectively.

* We use a compact universal circuit format which is composed of inputs, outputs, and 3 types of gates which we detail below. The input and output wires are at the beginning and the end of the UC files, recpectively.
```
C 0 1 2 3 ... N //Real input bits to the UC, i.e., x when the UC implements f(x).
O A B C ... //Output wires in order.
```
* The three gate types and their interpretations are detailed below. The X and Y switching blocks are interpreted as 1 AND and 3 or 2 XOR gates, respectively. All these gates use one line of the programming file, i.e., one bit for the switches or a 4-bit value for the universal gate.
```
/* X switch with A and B input wires and F and G output wires. 
Depending on the corresponding programming bit p (from programming file), 
it outputs either A and B in order (if p=0) or switched (if p=1). */
X A B F G (p)

/* Standard implementation of an X switch. */
XOR A B D //D = A XOR B
AND D p E //E = D AND p
XOR E A F //F = E XOR A
XOR E B G //G = E XOR B
```

```
/* Y switch with A and B input wires and F output wire. 
Depending on the corresponding programming bit p (from programming file), 
it outputs either A and B (if p=0 it outputs B, if p=1, it outputs A). */
Y A B F (p)

/* Standard implementation of a Y switch. */
XOR A B D //D = A XOR B
AND D p E //E = D AND p
XOR E A F //F = E XOR A
```

```
/* U universal gate with A and B input wires and Z output wire. 
Depending on the corresponding 4 programming bits p1, p2, p3, p4 (from programming file), 
it calculates the gate with the gate table p1, p2, p3, p4 with inputs A and B. */
U A B Z (p1 p2 p3 p4)

/* Implementation of a U gates based on Y gates. Note that here the programming bits of the Y gates are the inputs bits.*/
Y p1 p2 C (B)
Y p3 p4 D (B)
Y C D Z (A)
```

### Private Function Evaluation

  * Our UC compiler is compatible with ABY that can evaluate the UC generated. The code for this is available at the github page of [ABY](https://github.com/encryptogroup/ABY), under src/examples/uc_circuit.
  
