#!/usr/bin/env python3
import imaplib, email, os

# Connect to the Gmail 'Command Center'
mail = imaplib.IMAP4_SSL('imap.gmail.com')

# Use environment variables for credentials
email_user = os.environ.get('GMAIL_USER', 'your_email@gmail.com')
email_pass = os.environ.get('GMAIL_PASS', 'your_app_password')
mail.login(email_user, email_pass)
mail.select('inbox')

# Search for Jules' bug reports
status, messages = mail.search(None, 'FROM "Jules"')

for num in messages[0].split():
    res, msg = mail.fetch(num, '(RFC822)')
    for response in msg:
        if isinstance(response, tuple):
            msg = email.message_from_bytes(response[1])
            # Save directly to the 2TB Boneyard
            with open('/mnt/boneyard/logs/jules_honing_' + num.decode() + '.log', 'w') as f:
                f.write(str(msg.get_payload()))

mail.close()
