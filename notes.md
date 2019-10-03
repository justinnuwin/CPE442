# 9/26

In electronics we try to maximize performance and minimize power, energy, area, volume, and cost

Anatomy of RISC instruction
    - Alot of energy is used to get cache, registers, control overhead, and less than 2% is for the actual instruction itsself

TO cut down on waste on overhead, we use SIMD (single instruction, multiple data).

Data-level parallelism
- Scalar single operation, vector<n> n operations

benefit of vector instructions
- each instruction specifies more parallel work. 
- amortize instruction costs
    - small overhead energy for fetch and deode (for vector decoding)

Basic vector instructions: see slide


SIMD can offer about 10x benefit for some apps


Custom hardware: designed to run single alrogithim very fast and efficiently

Sobel Filter: 2D convolution filter on graysccale image
- Useful for edge detection and other CV based applications
- Performed per pixel
- 4 shifts, 10 adds/subs, 2 abs, 1 clamp

Sobel hardware
- Build memory structure suited to needs
- Stream in neighborhood of pixels

Convolution engine
- input stnecil

Memory concern: uaually bottleneck, use bandwidth matching
- Match performance with aavailable memory bandwidth
- Memory too slow: accellerator must stall or wait

Processor usually controls memory acellerator: will usually use DMA to read in data on circular buffer

Know what a DMA is and priciple of operation

Minimizing memory acces:
- Minimize read/writes by buffering operatnds and intermediates locally (in cache?)

For sobel, we can maximize memory reuse by buffering three rows of the image at a time


When to build custom:
- Design will be replicated many many times



# 10/3

This class all about optimizing algorithims
For example:
- Limited thermal envelope: Power optimization
- Limiter power supply: Energy optimization
- Limited ...

Will use sobel operator to as example of algorithim to optimize
- Sobel filter 2D convolution
- Utilizes Matrix multiplication by simple Kernel (pow2 square integer matrix)
    - Optimize by unrolling mat. mult.
    - Simplify multiplication and inversion operations (using shifts, etc)
        - You can depend on compiler to optimize this for you
- Computing gradient magnitudes is obscenely expensive (pow2 and sqrt!!)
    - Can approx by adding magnitudes of components

To parallelize sobel exmaple program:
- May try using threading 
