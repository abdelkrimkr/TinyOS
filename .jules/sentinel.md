## 2024-01-10 - Supply Chain Vulnerability in Build Tools
**Vulnerability:** The `setup_tools.sh` script downloaded Busybox and Limine binaries directly from the internet without verifying their integrity via checksums.
**Learning:** Even in minimal kernel projects, the build environment itself is a major attack vector. Compromised build tools can inject malicious code into the kernel binary or the host system during the build process.
**Prevention:** Always verify SHA256 (or better) checksums for all external dependencies downloaded during the build setup. The checksums should be pinned in the script and verified before execution or use.
