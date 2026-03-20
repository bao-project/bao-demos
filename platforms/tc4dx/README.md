# Infineon Tricore TC4DX COM

---

**NOTE**

We use TRACE32 platform to program the board. A generated .cmm script
will be available after the build to flash and run the demo.

---

<!--- instruction#1 -->
## 1) Setup board

Make sure you have the debug probe to connected to the board.
The all pins in the HW Boot Configuration Dipswitch should be off.

<!--- instruction#2 -->
## 2) Flash the demo

Run the following command:

` t32mtc -s ./platforms/tc4dx/startup.cmm `

<!--- instruction#end -->

The startup.cmm opens several windows inside TRACE32, including the main script
window. If you want to do it manually, open TRACE32 and select edit 
the template.cmm or the tc4_demo.cmm file 

<!--- instruction#3 -->
## 3) Run the script

Click GO on the central window.

<!--- instruction#end -->

Alternatively, if you want to re-flash and run the demo, select the option DO
on the window with the main script. And then, click GO on the main window to
run the demo.

<!-- Links -->

[AURIX-GCC]: https://softwaretools.infineon.com/assets/com.ifx.tb.tool.aurixgcc
