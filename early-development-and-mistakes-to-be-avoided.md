
### Appendix: early development and mistakes to be avoided

#### Nodogsplash
TollGate started off using an old `c` based captive portal called [nodogsplash](https://github.com/nodogsplash/nodogsplash).

The captive portal served a simple [HTLM page](https://github.com/OpenTollGate/tollgate-initial/blob/5614b3edcf77ef7c9b340f0e57ee67171619b60f/etc/nodogsplash/htdocs/splash.html#L24-L40) and [forwarded](https://github.com/OpenTollGate/tollgate-initial/blob/5614b3edcf77ef7c9b340f0e57ee67171619b60f/etc/config/nodogsplash#L78) any input strings that were submitted to a [script](https://github.com/OpenTollGate/tollgate-initial/blob/5614b3edcf77ef7c9b340f0e57ee67171619b60f/www/cgi-bin/curl_request.sh#L231-L235) that redeemed the e-cash and authenticated the user's session if the payment succeeded. The redeem script used an existing API called [redeem.cashu.me](https://redeem.cashu.me/) to melt e-cash notes to a given LNURL over lightning.

The benefit of using nodogsplash was the simplicity with which input strings could be passed to a script and the simplicity of the user facing HTML page. This made it rather straight forward to create a demo for TollGate. However, truncated input strings and a lack of control over session duration posed a challenge at the time.

[Increasing the length of the input fields](https://github.com/chGoodchild/nodogsplash/blob/09ba1b73566cea92640a35925c2e3a6bbdeeda46/src/http_microhttpd.c#L79-L80) that the captive portal could process allowed the strings to be long enough to process e-cash strings with `2^{n}` SAT denominations. These are the shortest e-cash notes because they only contain a single proof. Unfortunately other denominations like prime numbers that resulted in longer e-cash strings continued to get truncated despite my fix.

After blaming `microhttpd`, but failing to find the new bottleneck we gave up and tried a newer version of `nodogsplash` called `OpenNDS`. The issue may have simply been a limitation in the length of a scripts arguments in the shell terminal. This particular bottleneck was later resolved by writing the e-cash strings into a file and [passing the file path to the redeem script](https://github.com/OpenTollGate/tollgate-outdated-build-environment/blob/ebb875697ff7bb078382ece6107060f91e240809/files/cgi-bin/curl_request.sh#L58-L69). 

#### OpenNDS

* Benefits: could specify session expiration time in redeem script

* Issues: php & shell
* Lack of clear HTML
* FAS complexity
* Response of various client devices
* Luci doesn't work



#### Libsecp custom feed




https://github.com/openwrt/openwrt
	

https://github.com/chGoodchild/nodogsplash/blob/increase_username_field/README.md


#### Build process & reproducability