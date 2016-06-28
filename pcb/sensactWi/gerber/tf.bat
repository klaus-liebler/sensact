SETLOCAL ENABLEEXTENSIONS
SET me=%~n0
SET parent=%~dp0
SET base=sensactWi

SET EXT_OLD=-B.Cu.gbr
SET EXT_NEW=.GBL
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-F.Cu.gbr
SET EXT_NEW=.GTL
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-B.Mask.gbr
SET EXT_NEW=.GBS
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-F.Mask.gbr
SET EXT_NEW=.GTS
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-B.SilkS.gbr
SET EXT_NEW=.GBO
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-F.Paste.gbr
SET EXT_NEW=.GTP
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-B.Paste.gbr
SET EXT_NEW=.GBP
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-F.SilkS.gbr
SET EXT_NEW=.GTO
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=-Edge.Cuts.gbr
SET EXT_NEW=.GML
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%

SET EXT_OLD=.drl
SET EXT_NEW=.TXT
del %base%%EXT_NEW%
copy %base%%EXT_OLD% %base%%EXT_NEW%
pause