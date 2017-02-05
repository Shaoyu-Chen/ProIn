MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 112K
}

SECTIONS
{
	_empty_sp_ = ORIGIN(SRAM) + LENGTH(SRAM);
	_full_sp_ = _empty_sp_ - 1K;

	.text :
	{
		*(.vector_table)
		*(.text)
		*(.rodata)
		_lma_etext_ = .;
	} > FLASH

	.data : AT(_lma_etext_)
	{
		_vma_data_ = .;
		*(.data)
		_vma_edata_ = .;
	} > SRAM

	.spinlock :
	{
		_vma_spinlock_ = .;
		*(.spinlock)
		_vma_espinlock_ = .;
	} > SRAM

	.bss (NOLOAD) :
	{
		_vma_bss_ = .;
		*(.bss)
		_vma_ebss_ = .;
	} > SRAM

	. = ALIGN(8);
	_vma_heap_ = .;
	_vma_eheap_ = _full_sp_;	
}
