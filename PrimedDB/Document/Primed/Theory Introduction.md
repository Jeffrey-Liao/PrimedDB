*Primed database* is a classical database which is using prime decomposition feature to accelerate data query for all types of data.

The idea of prime data base is to convert each cell of data into a prime number. So that, a record in *Primed DB* is a number which is the multiplication result from all data in this record. The O(1) complexity query can be done for all types of data because each query is to mod record number with target number then check the result is 0 or not.

For instance we had a record below:

|             | name | phone number | student number | record number |
| ----------- | ---- | ------------ | -------------- | ------------- |
| actual data | Jeff | 1234         | 1111           | /             |
| prime key   | 7    | 11           | 13             | 1001          |
We can convert *Jeff* into 7 then do mod calculation with 1001 then check the result is zero or not when we want to query does *Jeff* is part of current record.

Also, the database will not give the original data to user but a record number. So that, potential attacker of the system will not get original data when he trying to get the query result illegally.

