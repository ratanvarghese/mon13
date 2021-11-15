local mon13 = require("mon13")

local mjd = mon13.mjdFromUnix(os.time())
print(mon13.format(mjd, mon13.gregorian, "%Y-%m-%d"))

local fmt = "%-d %B %Y, %q (%f)"
print(mon13.format(mjd, mon13.gregorian, mon13.gregorian_names_en_US, fmt))
print(mon13.format(mjd, mon13.tranquility, mon13.tranquility_names_en_US, fmt))
print(mon13.format(
	mjd,
	mon13.french_revolutionary_romme_sub1,
	mon13.french_revolutionary_names_fr_FR,
	fmt
))