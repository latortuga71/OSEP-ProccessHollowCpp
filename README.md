# README

Porting over c# process hollowing exercise from OSEP to cpp. After finishing sektor 7 intermediate malware course wanted to keep doing things in cpp rather than c# and they do not cover process hollowing in that course. This method doesnt really hollow out the process out it just overrites the entry point.

Using the metasploit encoder options break the payload so i used the new winapi for encrypting stuff to encrypt it. Then i decrypt it and overwrite the suspended svchost process.


