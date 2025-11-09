# Universal Service (uservice)

uservice is a small static program that you give systemd style commands and it will translate them to your installed init system

This is useful if you use many distros and swapping between syntax of init systems is annoying

## Supported Systems
- systemd
- openRC

If auto detection of your init system fails make a file at `/etc/uservice.conf` and plainly put in the name of the init system you wish to use as it is typed in the above list  
This also may speed up using uservice if auto detection on your system is slow

Config for systemd:
```
systemd
```

Config for openRC:
```
openRC
```

## Supported Operations
- `uservice start <service>`
- `uservice stop <service>`
- `uservice restart <service>`
- `uservice status <service>`
- `uservice enable <service>`
- `uservice disable <service>`

## Compiling and Installing
Download the repo and run `make` to compile  
Copy the resulting binary to `/bin`
You may or may not need to restart your shell

If you are updating just redownload and repeat the steps and overwrite the file

## Notes
- On OpenRC enabling and disabling services always happens on runlevel `default`
