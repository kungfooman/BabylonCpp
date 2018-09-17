const fastcall = require('fastcall');
const Library = fastcall.Library;
const ref = fastcall.ref;
const lib = new Library('SampleLauncher.dll')

lib.function('int launcher_main()')

lib.interface.launcher_main()