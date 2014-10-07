Arnold extensions by Troels K. 2003-12-08
- Save screenshot as PNG
- Alternative color scheme.
- Unicode enabled: Replaced char with TCHAR wherever possible. Affects most files in the project. (arnoldw.exe)
- Autorun feature. Usage: arnold.exe "mode 2:cat"
- 'Internal' ROM feature. (Arnor ROMs: "<protext.rom>", "<utopia.rom>")
- Using precompiled headers.
- Console (text) mode option. Turns off all DirectX functionality. (arnold.exe -console)
- MFC demos: TelnetCPC, Edit


Folders:

\arnold  - Arnold project
\arnoldx - Projects derived from Arnold (Edit, TelnetCPC)
\png     - libpng. Required by Arnold
\unzip   - unzip. Required by Arnold
\zlib    - zlib. Required by libpng and unzip
\ioapi   - unzip\ioapi, unicode version. Required by Arnold
\debug   - Intermediate files
\release - Intermediate files
\bin     - Binaries
