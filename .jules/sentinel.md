## 2024-10-24 - Supply Chain Vulnerability: Missing Checksum Verification
**Vulnerability:** The `setup_tools.sh` script downloaded build dependencies (`busybox`, `limine`) without verifying their SHA256 checksums.
**Learning:** Build scripts are often overlooked in security audits but are critical injection points for supply chain attacks. Trusting downloads over HTTP/HTTPS without verification exposes the developer to compromised servers or MITM attacks.
**Prevention:** Always hardcode expected cryptographic hashes (SHA256 or better) for all third-party binaries or libraries downloaded during the build process. Implement 'Trust On First Use' if official checksums are unavailable, then verify independently.
