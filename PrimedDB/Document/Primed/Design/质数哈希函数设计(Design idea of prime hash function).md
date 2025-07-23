# 2025-06-27
通过已有哈希函数算法生成数据的唯一哈希值
然后通过映射将当前值映射到更大的数据空间
*(AI generated)*
*"Generate a unique hash value of the data using an existing hash function algorithm, and then map the current value to a larger data space through mapping."*

# 2025-06-30
找到开源库CryptoPP并使用其中的算法实现生成SHA-256哈希键。但鉴于即使是256位的数据转为数字后都很难进行高效的取模和寻找质数运算。所以利用C++的指针特性，将256位的数据截取为$32*8$的int数组，然后将每个独立的int数据映射到到指定的$2^n$数据空间（使用GMP开源代码库完成）
*(AI generated)
Find the open-source library CryptoPP and use its algorithms to implement the generation of SHA-256 hash keys. However, since even 256-bit data, when converted to numbers, makes it difficult to perform efficient modulo and prime number finding operations, we take advantage of C++'s pointer feature to split the 256-bit data into an int array of $32bit \cdot 8$, and then map each independent int data to a specified $2^n$ data space (using the GMP open-source code library to complete).*

经过测试，SHA256生成的字符串被分组后的int值基本都在1000以下。可以生成8个独立的质数键组合后成为SHA256的整体键。

对于普通数据(32位int)和截取SHA256的段数据，将其映射到$x*2^{16}$空间可以有效的防止质数哈希键冲突。
在将其映射到$x*2^{8}$空间使用均匀分布进行7千万次暴力冲突测试时出现了质数哈希键冲突：

*(AI generated)
After testing, the int values of the SHA256-generated strings after being grouped are mostly below 1000. Eight independent prime number keys can be generated and combined to form the overall key of SHA256.*  
  
*(AI generated but fixed by myself)
For ordinary data (32-bit int) and truncated SHA256 segment data, mapping them to space  $x*2^{16}$ can effectively prevent prime hash key conflicts. However, there has hash conflicts on $2^{8}$ level space enlarge mapping when conducting 70 million brute-force conflict tests using a uniform distribution for mapping to space.*


在第**1604220**次循环中，键==165164923397==同时会被映射到`1290350964`和`1290350963`
在第**1366598**次循环中，键==170321113619 ==同时会被映射到`1330633700`和`1330633699`


*Key ==165164923397== is prime hash key for both `1290350964` and `1290350963` at iteration No:1604220 when we are doing brute-force testing to the algorithm.
*Key ==170321113619== is prime hash key for both `1330633700` and `1330633699` at iteration No:1604220 when we are doing brute-force testing to the algorithm.*
# 2025-07-01
基于昨天的实验结果，今天对于$x*(2^{8} \sim 2^{16})$之间的其他方案进行测试，以找到最小的无冲突映射规则。同时，今天会对long long大小的数据进行测试，以测试所对应的数量级的最大碰撞能力。


同时，基于素数定理（哈代数论\[An introduction to the theory of numbers | by G.H.Hardy(UK) E.M. Wright(UK), page 8(Chinese translated version)\]）,即小于$x$的素数数量为$\pi(x)$, 而$\pi(x) \sim \frac{x}{ln(x)}$ 。

### 质数扩张空间(Prime extend Space)$ps$和质数空间大小(Prime number)$pn$
所以我们可以得出将$2^{64}$大小的数据映射到新的质数空间所产生的可能得质数数量为$pn(x) =  \frac{x\cdot2^n}{ln(x\cdot2^n)} - \frac{2^n} {ln(2^{n})} + 1$ ，n为映射到新空间的位数指数。产生的新的质数空间的范围为$ps(x) = \{p| (x\cdot 2^{n} \ge p\ge 2^{n} \lor p = 2)\land is\_prime(p) \}$。
由表达式可得 $p$ 的取值下界是：当给定的 $x$ 为1时，$x \cdot 2^n$ 的下一个质数。而它的取值上界是当给定的$x$为$x$的最大值时：$x\cdot 2^{n}$ 的上一个质数。当然，还有一个特例，就是$x$ 为0时，$p$的取值为0的下一个质数2.

### 质数键碰撞测试实验(Prime Conflict Test)
此外，我们需要探寻到不同方案中质数碰撞的可能性。因此设计了一种试验方法方法：使用均匀分布的随机数生成器`uniform_int_distribution`进行n次(n尽可能大)的质数生成测试，记录下重复的原数字$n1,n2$ 和生成的质数键$p$。然后去除重复的情况统计冲突次数/总循环次数得出大致的碰撞概率

### 质数覆盖占比(Prime coverage)$pc$
使用区间可以生成的质数总数除以原空间总数字数量，可以看出当前映射有没有能力可以覆盖全部的数字空间。

如果$pc\lt1$则证明当前空间没有能力覆盖原有的数据空间，则不需要进行质数碰撞测试就可以得出该空间无法容纳原有数据空间的质数键生成要求。

如果$pc>1$则证明当前质数空间中的质数相比之前的空间内的总数字数量扩张了多少倍
$pc(pn)=\frac{pn}{2^n}$

| $n$ | $pn({2^{63}}) = \pi(x*2^n)-\pi(2^n)+1$ | PCT(>1.2亿次)    | $pc=\frac{pn}{x^n}$                   |
| --- | -------------------------------------- | -------------- | ------------------------------------- |
| 7   | $2.43*10^{19}$                         | $0.29*10^{-2}$ | $\frac{2.43*10^{19}}{2^{63}} = 2.63$  |
| 8   | $4.80*10^{19}$                         | $0.97*10^{-3}$ | $\frac {4.80*10^{19}}{2^{63}}=5.20$   |
| 9   | $9.46*10^{19}$                         | $0.18*10^{-5}$ | $\frac {9.46*10^{19}}{2^{63}}=10.26$  |
| 10  | $1.87*10^{20}$                         | 0(3亿次)         | $\frac{1.87*10^{20}}{2^{63}} = 20.27$ |

# 2025-07-02
关于如何证明质数在指定的$2^{n}$扩张空间不碰撞以及如何计算质数在指定空间的碰撞概率仍然在思考中，但我们可以找到基于质数哈希键和原数据的关系。
```
Number: 19171246 Hash: 19631355907 Decode: 19171246  1
Number: 13636858 Hash: 13964142607 Decode: 13636858  1
Number: 390629 Hash: 400004107 Decode: 390629  1
Number: 13402639 Hash: 13724302369 Decode: 13402639  1
Number: 4210104 Hash: 4311146537 Decode: 4210104  1
Number: 10140414 Hash: 10383783959 Decode: 10140414  1
Number: 11600567 Hash: 11878980611 Decode: 11600567  1
Number: 8567359 Hash: 8772975637 Decode: 8567359  1
Number: 3554956 Hash: 3640274971 Decode: 3554956  1
Number: 18568331 Hash: 19013970961 Decode: 18568331  1
Number: 8877040 Hash: 9090088979 Decode: 8877040  1
Number: 2232383 Hash: 2285960213 Decode: 2232383  1
Number: 12488995 Hash: 12788730883 Decode: 12488995  1
Number: 16712554 Hash: 17113655309 Decode: 16712554  1
Number: 10365718 Hash: 10614495251 Decode: 10365718  1
Number: 15807837 Hash: 16187225099 Decode: 15807837  1
Number: 17130348 Hash: 17541476383 Decode: 17130348  1
Number: 17212803 Hash: 17625910277 Decode: 17212803  1
Number: 6330372 Hash: 6482300947 Decode: 6330372  1
Number: 14366902 Hash: 14711707651 Decode: 14366902  1
```
在测试的1万条数据中，哈希键可以完美的通过$\frac{p}{2^n}$这个公式还原出原始数据。这样对于数据的加密和解密可以被轻松的在O(1)时间内完成，不需要额外的算法或者存储空间（如哈希表）。

# 2025-07-03
基于前几日的研究结果，我们现在需要去探究以下几个问题：
1. 如何证明，通过映射关系$p = f(x) \land is\_prime(p)$ 产生的p对于相邻的两个$x_1,x_2 \land \lvert x_1-x_2 \rvert= 1$完全不会产生冲突。以及，如果有产生冲突的可能性，产生冲突的概率是多少？
2. 为何加密后的密文可以轻松地通过数学运算实现解密，也即，如何证明这个算法是一个满射关系，以及这个满射关系关于原空间和映射空间以及映射关系成立的条件。
3. 如何找到最小的$n$使得加解密所需的时间和空间成本都降到最低
4. 能否找到其他的映射空间底数使其比2为底的映射空间底数更高效和更简单
5. 证明这种映射关系是仅在2为底的映射底数上成立还是在其他底数上也成立。并探究其原因
# 2025-07-03