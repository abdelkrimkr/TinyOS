## 2026-01-11 - Supply Chain Security: Tool Verification
**Vulnerability:** The build script `setup_tools.sh` downloaded binaries (Busybox, Limine) over the internet without verifying their integrity (checksums).
**Learning:** Even in minimal projects, external dependencies downloaded at build time are a significant supply chain risk. Attackers could compromise the download server or perform MITM attacks to serve malicious binaries.
**Prevention:** Always verify SHA256 checksums of downloaded artifacts against a known good value (pinned checksums). Use "Trust On First Use" if official checksums are unavailable, to at least pin the current state.
