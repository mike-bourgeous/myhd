### Import from CVS to Git

```bash
rsync -av rsync://myhd.cvs.sf.net/cvsroot/ /tmp/myhd-cvs
echo 'nitrogen=Mike Bourgeous <mike@mikebourgeous.com> -0600' > /tmp/myhd-cvs/cvs_authors
mkdir /tmp/myhd
cd /tmp/myhd
git cvsimport -A /tmp/myhd-cvs/cvs_authors -d /tmp/myhd-cvs myhd
```
