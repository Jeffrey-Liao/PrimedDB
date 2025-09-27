# What is Primize
As the name said, Primize is an key algorithm of primed database which turning allowed data type input into a prime number. It also differentiate Primed database from traditional relation database products.

# The mechanism of Primize
The concept of Primize is simple that Primize will trim given data into a 4 bytes integer or many 4 bytes integers. After that, the Primize will enlarge each integer by $2^9$ because the number $2^9$(512) is greater than the biggest gap (336) between primes on domain $[0,2^{32}]$.

