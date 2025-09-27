# What is Primed?
Primed is a relation database which turning all types of data into a prime number. Thus, all types data is able to encrypt since the time that this data added into Primed database. Furthermore, a record is represent by:
1. Record number, it is not a unique number which produced by multiplication to all attributes of current record. There could exist multiple same record number if all data in that record are same.
2. Primed data, all types of data in primed database will be converted into a unique prime number by 'Primize' algorithm which will be elaborately explained later.
# Why Primed
**Firstly**, the query efficiency of primed database is much higher than traditional relation database theoretically because the ideal time complexity of mod calculation is O(1) but O(n) for string comparison. **Furthermore**, the query operation can only scan and compare few bytes instead of all memory of a record due to the existence of record number which can recognise all data components by prime factorization.
**Secondly**, the data is totally safe and 'encryptable' as all types data are converted into prime which are easy to encrypt. Thus, the potential attacker cannot get data directly from data file.

