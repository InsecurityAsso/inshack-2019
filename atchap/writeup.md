# Almost Tchap

If you have followed the french infosec news lately, this challenge might sound familiar. Indeed, it is based on a vuln found by a french searcher on the new french communication app based on matrix. 

Basically you just have to enter any mail address on the format : mymail@address.com@legit.name@almosttchap.fr to receive the flag. legit.name should take the format of one of the people on the website : for example samira.bien@almosttchap.fr. Obviously you need to setup a burplike sotware to bypass the in browser validation.