#include <kernel/mm/virtualPageManager.h>
#include <kernel/acpi/rsdp.h>
#include <lib/string.h>
#include <lib/output.h>

#include <stddef.h>

rsdp_t *rsdp = NULL;
rsdt_t *rsdt = NULL;
xsdt_t *xsdt = NULL;

void rsdpInit(uint64_t *rsdpAddr) {
    rsdp = (rsdp_t*)rsdpAddr;
    if(rsdp->xsdtAddr) {
        xsdt = (xsdt_t*)((uint64_t)rsdp->xsdtAddr);
        kprintDS("[ACPI]", "xsdt located at %x", xsdt);
        return;
    }

    if(rsdp->rsdtAddr) {
        rsdt = (rsdt_t*)((uint64_t)rsdp->rsdtAddr + HIGH_VMA);
        kprintDS("[ACPI]", "rsdt located at %x", rsdt);
        return;
    }
}

void *findSDT(char *signature) {
    if(xsdt != NULL) {
        for(uint64_t i = 0; i < (xsdt->acpihdr.length - sizeof(acpihdr_t)); i++) {
            acpihdr_t *acpihdr = (acpihdr_t*)(xsdt->acpiptr[i] + HIGH_VMA);
            if(strncmp(acpihdr->signature, signature, 4) == 0) {
                kprintDS("[ACPI]", "%s located at %x", signature, acpihdr);
                return (void*)acpihdr;
            }
        }
    } 

    if(rsdt != NULL) {
        for(uint64_t i = 0; i < (rsdt->acpihdr.length - sizeof(acpihdr_t)); i++) {
            acpihdr_t *acpihdr = (acpihdr_t*)((uint64_t)rsdt->acpiptr[i] + HIGH_VMA);
            if(strncmp(acpihdr->signature, signature, 4) == 0) {
                kprintDS("[ACPI]", "%s located at %x", signature, acpihdr);
                return (void*)acpihdr;
            }
        }
    }

    kprintDS("[ACPI]", "%s could not be found :(", signature);
}
