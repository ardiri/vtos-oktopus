OUTPUT_FORMAT("ecoff-littlemips")
ENTRY(appstart)

MEMORY
{
	FLASH   : org = 0x9FC30000, l = 1M
	RAM	: org = 0x10000000, l = 1M
}

SECTIONS
{
	.head :
	{
		_fhead = . ;
		appstart.o(.data)
		ehead = . ;
	} > RAM

	.text :
	{
    	_ftext = . ;
    		appstart.o(.text)
    		*(.text)
    		resource.o(.data)
    	etext = . ;
	} > RAM

	.data ALIGN(0x1000) :
	{
		_fdata = . ;
		*(.data)
		*(.rdata)
		edata = . ;
	} > RAM

	.bss :
	{
		_fbss = . ;
		*(.bss)
		*(COMMON)
		*(.common)
		*(.scommon)
		ebss = . ;
	} > RAM
}
