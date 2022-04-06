
#### TaskDialog - moving beyond the MessageBox.

This an exploration of the [TaskDialog][1] addition to Windows UI programming, and patterns in [fasmg][2].

[Kenny Kerr][3] put together the original sample program in C++ when Windows Vista was still in beta. I thought it was such a good idea that I've mirrored his effort in x86-64 assembly. Obviously, we don't need ATL/WTL or the wrapper he created - it's mostly just [SendMessage][4].

Often lines of code in assembly are comparable given the right model. In fasmg we can construct domain models fitting a large array of problems.


![GitHub repo size][6] ![GitHub code size in bytes][5]


[1]: https://docs.microsoft.com/en-us/windows/win32/controls/task-dialogs-overview
[2]: https://github.com/tgrysztar/fasmg
[3]: https://weblogs.asp.net/kennykerr/Windows-Vista-for-Developers-_1320_-Part-2-_1320_-Task-Dialogs-in-Depth
[4]: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessage
[5]: https://img.shields.io/github/languages/code-size/bitRAKE/fasmg-TaskDialog?style=for-the-badge
[6]: https://img.shields.io/github/repo-size/bitRAKE/fasmg-TaskDialog?style=for-the-badge
