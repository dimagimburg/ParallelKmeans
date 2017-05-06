# Parallel and sequential implementation of cluster finding algorithm
based on KMeans, parallel is implemented with CUDA (not yet implemented) and OpenMP,
communication is done with MPI (mpich1.4.1p)

## Installation
(if you use visual studio) link the MPICH library follow those instructions:
1. http://nick-goodman.blogspot.co.il/2012/02/using-mpich-from-ms-visual-studio-2010.html
2. http://www.screencast.com/t/YwAEXhPlMT (make sure you are using x64 as platform)

## The problem world
*impleneted for points [(x,y) plane], each point is moving in circle around its center*

Given input is:

1. **N** - number of points (a, b, radius)
2. **K** - number of clusters wanted
2. **delta_t** - the time step between calculation of centers of the clusters
3. **T** - time interval
4. **LIMIT** - the max number of iterations fo KMeans alg.

![Point p moves](https://s18.postimg.org/jdsa8btah/2016_10_19_1119.png)

input is delivered from file looks like:

**line 1:** N K delta_t T LIMIT

**line 2 - line N:** point_index x y radius

## Input
provided in this repository in file: kmeans10.txt (300k points)

**line 1:** N K delta_t T LIMIT

**line 2 - line N:** point_index x y radius

## Output
Output is a file with the delta_t where the clusters centers were the closest to each other, format
example:

*d = 14.5*

*delta_t = 120.3*

*centers of cluster:*

*12.4 55.6*

*2.4 90.15*

*66.0 45.2*

## Implementation
Kmeans algorithm is implemented in its naive way delta_t / T number of times. Pseudo code:

```
for each delta_t of T do
	for n to LIMIT do:
		for each cluster out of k clusters do
			number_of_points = 0
			set_initial_center_to_be_the_next_point_of_the_points_array()
			set_current_centers_array() // for later comparison if cluster changed or not

		for each current_point out of points array do
			min_distance = get_distance_from_cluster(0)
			min_cluster = cluster_0
			for clusters c 1 to N do:
				new_distance = get_distance_from_cluster(c)
				if new_distance < min_distance
					min_distance = new_distance
					min_cluster = c
			add_point_to_cluster(current_point, min_cluster)

		for each cluster c out of N do:
			cluster_recenter(c) // goes along all of local clusters points and calculate center

		not_changed_clusters = 0 
		foreach j = 0 to K (number of clusters) do
			If centers are close in approximation
				not_changed_clusters++

		If not_changed_clusters = K
			return
```

## Complexity
*number_of_increments = delta_t / T*

*num_points = n*

*num_clusters = k*

*limit = LIMIT*

**O(number_of_increments * num_points * num_clusters * limit )**

## Adding parallelism with MPI library

The idea is to distribute the hard work which is the KMeans algorithm itself between processes. 
Each process (can be hosted on different CPU core or different machine, simulated on windows 
OS with `winmpiexec.exe` application) will be in charge of a chunk of work of the whole problem scope. 
The `MASTER process (id=0)` will get the input data and will distribute it to all other processes (broadcast). 
Each process based on simple calculation will get number of increments (chunks of `delta_t` of the `time T`) 
and perform the KMeans algorithm on his chunks. After all processes finished, `MASTE`R gathers the results
which are represented in a special information data type to include only relevant data for the results, 
and makes the calculation to find the closest clusters center as described in the scope of problem section. 
`MASTER` is the one who writes results into a final results file.

Basically what `MPI` parallelism improves in the complexity is the `number_of_increments`, which is basically 
in the context of the problem scope could be reduced to `1`. MPI initialization and data transferring constant 
is added but of course it is a constant so when scale up we can ignore it. And the complexity turns into:

**O((num_points * num_clusters * limit) + MPI_INIT_CONSTANT)**

## Tests
###### The test were made on a machine with 4 cores and the following specifications:
![PC specs](https://content.screencast.com/users/dimagimburg/folders/Jing/media/bee32bc0-59b1-47d8-8b33-57fecaf2f105/2017-05-06_1048.png)

###### Tests with mpi initialization 
![Tests with MPI](https://content.screencast.com/users/dimagimburg/folders/Jing/media/851581d7-b100-4dbe-a66e-375e7f942e91/2017-05-06_1050.png)

![Tests with MPI graph](https://content.screencast.com/users/dimagimburg/folders/Jing/media/789e1727-8055-4001-981d-c74e85b83d7b/2017-05-06_1051.png)

From the results above we can see that mpi parallelism is helping us to gain much more computing power when number of processes grow respectively with the number of cores.
We can see though, when number of processes used to calculate is equal or bigger than number of core on your machine, 
results tend to be inconsistent and more random, that happens because the core the OS using in order manage the OS I/O 
depends on the I/O itself, means when there are more interrupts in the background, more context switches, resulting 
more time to compute.

## Adding parallelism with OPENMP

To get even more impressive results I've added `OPENMP` library parallelism to the calculation of the KMeans algorithm 
which took most of the process power in the program. The limitation of the OPENMP library is that you need to find 
places where you can use it, that means it fits only to pieces of code that are loosely coupled and have no effect 
on each other that can lead you to a deadlock. That means that I couldn't add it to pieces of code where I make 
allocation of memory where the next operation depends on the previous one. For those places I could add parallelism 
with CUDA, which is yet to be implemented and is on the to-do list. The place where I could add it is on the step where 
the algorithm calculates the centers of the clusters, it is great for running on for loops and was easy to implement. 
This piece of code runs each time it invoked on the whole set of moving points attached to the current cluster, and 
calculates the center of it by simple avg function, although it seems as simple function it could possibly run on the 
whole N points and so parallelism is really necessary there. We can also see that adding the support for OPENMP library 
adds some more complexity in the initialization of the program and hence adds some more time for the algorithm when 
running on low number of processes, as we add more processes we can see that the results getting better and better so 
the addition of the library on the long run improves the results but not as we would expect. In my opinion I should look 
for more ways to parallelize the code, make it more uncoupled and then the effect will be much better:

![Tests with OMP](https://content.screencast.com/users/dimagimburg/folders/Jing/media/5f143972-af55-4d2e-b385-d740dd96b139/2017-05-06_1055.png)

![Tests with OMP graph](https://content.screencast.com/users/dimagimburg/folders/Jing/media/271baed0-860c-4f00-8c2f-dd45b14187ef/2017-05-06_1056.png)
