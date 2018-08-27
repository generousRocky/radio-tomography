<div class="header" align="center">
	<h2>
		<a href="https://rockylim92.github.io/" title="temp">
			<img alt="" src="https://github.com/RockyLim92/copyBox/blob/master/asset/rocky_icon.png" width="100px" height="100px" />
		</a>
		<br />
		Radio Tomographic Imaging using Contiki OS on CC2650
	</h2>
	<p align="center">:octocat: from device application to reconstruction algorithm :octocat:</p>
</div>

---

<div class="header" align="center">
<a href="https://www.youtube.com/watch?v=A1ZUN6HhKXg" title="temp">
<img alt="" src="https://rockylim92.github.io/images/csiro_6_0.png" width="650px" />
</a>
</div>


### Overview
`[Simple Description]`

Radio Tomographic Imaging (RTI) is a technology that uses a sensor network system to image moving objects within a specific range. The Received Signal Strength Index (RRSI), which is the strength of radio signals sent and received by network equipment, is interrupted by objects between two network devices and is reduced in size. Therefore, by deploying a sufficient number of sensor network devices (nodes) within a specific space and analyzing the RRSI values ​​between each pair of nodes, images similar to those of a thermal imaging camera can be generated. [1]

    In the course of this project, we implemented a full-stack for RTI System and successfully demoed and merged code. The RTI System is mainly divided into a system for sensor network devices and a server for generating images by analyzing signals generated from the sensor network nodes.

  For sensor network scheduling, a sensor network node operating on Contiki-OS, a real-time OS, was deployed and a scheduling algorithm was developed to schedule operation between these nodes. We have also modified the network stack of Contiki-OS to create images with sufficient resolution and image frequency, rather than simply developing network applications based on Contiki-OS. Existing bare-metal Contiki-OS was able to send and receive 8 radio signals per second, but the validation in the network stack was moved to the application layer and improved to show more than 60 times per second. As a result, we built a sensor network system that is robust against network failures and exhibits good performance. [4]

    In the Image Reconstruction step, the reconstruction algorithm was implemented in Python code by referring to the mathematical model proposed in the previous study [1], and the image was created by Parameter optimization process for the used device SensorTag CC2650 [2] . [3]

The full stack code is accessible from the CSIRO private repository. The main algorithms and the code for the reconstruction can be found at https://github.com/RockyLim92/radio-tomography. The project demo video is available at www.youtube.com/watch?v=A1ZUN6HhKXg.



### Contributor(s)
- **Rocky Lim** - [GitHub](https://github.com/RockyLim92)


### Development
- Contiki Application
- Simplified and optimized network stack of Contiki OS
- Implementation of image re-construction algorithm


### Build (or Installation)
Follow the instructions on [https://rockylim92.github.io/research/csiro03/]
<!--
```
~$ sudo apt-get update
```
-->

### Usage  
Follow the instructions on 

[https://rockylim92.github.io/research/csiro04/]

[https://rockylim92.github.io/research/csiro05/]

[https://rockylim92.github.io/research/csiro06/]

<!--
```
~$ sudo apt-get update
```
-->

### Contents
`[Directory Description]`
- contiki-examples/rti_master/ - master node
- contiki-examples/radio_tomograph/ - slave nodes
- reconstruct/ - python codes for image reconstruction
- etc


### Acknowledgments
- brano
- Joan
- Jin

### References
[1]: Wilson, Joey, and Neal Patwari. & Quot; Radio tomographic imaging with wireless networks. & Quot; IEEE Transactions on Mobile Computing 9.5 (2010): 621-632.

[2]: rockylim92.github.io/research/csiro02

[3]: rockylim92.github.io/research/csiro06

[4]: rockylim92.github.io/research/csiro04
