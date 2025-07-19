# FVP-R

## 1) Download and install the FVP model

---

**NOTE**

If you already have FVP_BaseR_AEMv8R installed please make sure you are
using version 11.28_23 or higher. If so, you can skip this step.

---

Download and extract the model:

```
curl -L https://developer.arm.com/-/cdn-downloads/permalink/FVPs-Architecture/FM-11.28/FVP_Base_AEMv8R_11.28_23_Linux64.tgz | tar xz -C $BAO_DEMOS_WRKDIR_PLAT
export PATH=$PATH:$BAO_DEMOS_WRKDIR_PLAT/AEMv8R_base_pkg/models/Linux64_GCC-9.3
```

## 2) Run FVP

```
FVP_BaseR_AEMv8R \
	-C cache_state_modelled=0 \
	-C bp.refcounter.use_real_time=1 \
	-C bp.exclusive_monitor.monitor_access_level=1 \
	-C cci400.force_on_from_start=1 \
	-C gic_distributor.has-two-security-states=0 \
	-C cluster0.gicv3.cpuintf-mmap-access-level=2 \
	-C cluster0.gicv3.SRE-EL2-enable-RAO=1 \
    -C cluster0.has_aarch64=$(([[ $ARCH == aarch64 ]]) && echo 1 || echo 0) \
	-C cluster0.VMSA_supported=$(([[ $ARCH == aarch64 ]]) && echo 1 || echo 0) \
	-C cluster0.gicv3.SRE-enable-action-on-mmap=$(([[ $ARCH == aarch64 ]]) && echo 2 || echo 0) \
	-C cluster0.gicv3.extended-interrupt-range-support=$(([[ $ARCH == aarch64 ]]) && echo 1 || echo 0) \
	-C bp.smsc_91c111.enabled=true -C bp.hostbridge.userNetworking=true \
	-C bp.hostbridge.userNetSubnet=192.168.42.0/24 -C bp.hostbridge.userNetPorts=127.0.0.1:5555=22 \
    --data==$BAO_DEMOS_WRKDIR_IMGS/bao.bin@0x0 $(printf -- "--data=%s " $FVPR_VM_IMAGES)
```

---
<!--- instruction#1 -->
**NOTE**

The FVP AEM models are quite slow. Demos featuring heavier guests like Linux
might take some time to fully boot up.
<!--- instruction#end -->
---
