# April-Fools-Proxy
This proxy will generate random spelling mistakes in webpage. 

______________________________________________________________________________________________________________________________
In order to compile the code you can type the following on terminal: “g++ proxy.cpp –o porxy” and to run the code type: “./porxy x”, where x is a number that determines the number of mistakes in the webpage. 
I am currently using Firefox as my web browser for testing. To configure the web browser you must go to the setting and search “proxy”. Change the HTTP Proxy to 127.0.0.1 and the port number to 8001. After every time you load a page, you must clear the cache. 
The proxy will give you a modified webpage with x number of mistakes in it. If the page is a HTML page, the mistake can be in the title of the page as well as the body. In a TXT page, the mistake will appear in the text itself. 
The testing was all done on my MacBook pro using Firefox. All the test cases that were provided currently work except for the gif. If the page is not a HTML or TXT, it will not modify the page just send it to the browser as it received it. 
