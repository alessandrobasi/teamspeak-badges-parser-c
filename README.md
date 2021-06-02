# Teamspeak Badges Parser

Small program to view old and "incoming" new badges of TeamSpeak

# Information source

> https://badges-content.teamspeak.com/list

# Heart of the program

```regex
^\$(?P<guid>[\w\d-]+).(.|\n)(?P<nome>.+)..(?P<url>https://[\w\-\.\/]+)..(?P<desc>\w.*)\(.+$
```

More info on: https://regex101.com/r/rnyijt/1


# License & Copyright

Program distributed under GPLv3 license

> Data and badges images, Copyright TeamSpeak Systems GmbH

> This software use OpenSSL, Apache license