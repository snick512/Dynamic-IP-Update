# Dynamic-IP-Update
 A C program to idle, and update ufw based on the IPv4 of a domain.

 Wrote by AI, directed by Ty Clifford.

 # Setup

 Use whatever DNS provider you wish. Set an "A" record to the IPv4 you want to be updated in the host's ufw. 

 ## Compile the Program

```bash
gcc -o ipupdate ipupdate.c
```

Creating the .service:
```bash
sudo nano /etc/systemd/system/ip-update-ufw.service
```

Insert: 
```ini
[Unit]
Description=UFW Updater Service
After=network.target

[Service]
ExecStart=/path/to/ip-update_ufw_service <domain>
Restart=always
User=root
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=update-ufw
KillSignal=SIGTERM

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload
```
Enable:
```bash
sudo systemctl enable ip-update-ufw.service
```

Start:
```bash
sudo systemctl start ip-update-ufw.service
```

Status:
```bash
sudo systemctl status ip-update-ufw.service
```

Logging/Debugging:
```bash
sudo journalctl -u ip-update-ufw.service
```

# Why?
Sometimes old fashion programs/scripts are the best. Some things have become so bloated they either become useless, or become unfit for practical use when you're looking for simplicity. 

Stay up to date on full blurbs at [my blog](https://isnick.net). 