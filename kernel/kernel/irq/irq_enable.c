#include <asm/asm.h>
#include <cpu/irq.h>
#include <vga/vga.h>

int irq_is_enabled = 0;
extern irq_is_disabled;

int irq_enable() {
  if (irq_is_disabled == 0 && irq_is_enabled == 1) {
    writestr("irqs already enabled");
    return 1;
  }

  _asm("sti");
  irq_is_enabled = 1;
}
