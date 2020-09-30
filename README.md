# OCTPP-Chess-2015
A complete game of chess with online and multiplayer capabilities. This started off as a class 12 Computer Science project, and rapidly grew to its current size!

#### Prerequisites for compiling source code
- Windows 7 and above
- Microsoft Visual Studio 2013 or later

#### To install wxWidgets
- Follow the instructions on http://liuxingguang.blogspot.in/2014/02/using-wxwidgets-30-in-microsoft-visual.html

#### Start the project
- Follow the instructions at the end of http://liuxingguang.blogspot.in/2014/02/using-wxwidgets-30-in-microsoft-visual.html to create a new Visual Studio Project called OCTPP.
- Copy the source-code from here into the CPP file of the Visual Studio Project you just created.

#### Installing cURL and libcurl
- Download <strong>libcurl-7.19.3-win32-ssl-msvc.zip</strong> from http://curl.haxx.se/download/trash/
- Extract all files to a directory without using spaces.
- Open OCTPP (the project _you_ created) in Visual Studio.
- In the Solution Explorer, right-click on the project file and select <strong>Properties</strong>.
- Under the tab Configuration Properties -> VC++ Directories, add the "include" folder of the cURL download to the "Include Directories" field. Also add the path of "lib/Release" in the cURL folder to the "Library Directories" field.
- For good measure, under the C/C++ tab, add the Include directory like you did in the last step.
- Similarly, open the Linker tab and add the library directory from cURL under "Additional Library Directories".
- Finally, expand Linker in the properties. Under the "Input" tab, make sure "curllib.lib" is one of the "Additional Dependencies". If not, then add it.

#### Building the project
- Hopefully, if all goes well, the project will build successfully. If necessary, copy the cURL DLL files into the project directory. If "libsasl.dll" is missing, download it from http://www.dll-files.com/dllindex/dll-files.shtml?libsasl.


#### Contact Us
- If something goes wrong, or for any other queries, feel free to send an email to support@octpp.tk.
