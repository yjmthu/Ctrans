import urllib.request
import urllib.parse
import hashlib
import time
import json
import uuid
import sys

# -------------------------------------------------------

def encrypt(signStr):
    hash_algorithm = hashlib.sha256()
    hash_algorithm.update(signStr.encode('utf-8'))
    return hash_algorithm.hexdigest()


def truncate(q):
    if q is None:
        return None
    size = len(q)
    return q if size <= 20 else q[0:10] + str(size) + q[size - 10:size]


def youdao(text):
    APP_KEY = '0b5f90d14623b917'
    APP_SECRET = '8X1HcIvXXETCRf2smIbey8AGJ2xGRyK3'
    data = {}
    data['from'] = 'auto'
    data['to'] = 'auto'
    data['signType'] = 'v3'
    curtime = str(int(time.time()))
    data['curtime'] = curtime
    salt = str(uuid.uuid1())
    signStr = APP_KEY + truncate(text) + salt + curtime + APP_SECRET
    sign = encrypt(signStr)
    data['appKey'] = APP_KEY
    data['q'] = text
    data['salt'] = salt
    data['sign'] = sign

    data = urllib.parse.urlencode(data).encode('utf-8')
    url = 'http://openapi.youdao.com/api'
    print(url + "/?" + str(data))
    headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    req = urllib.request.Request(url=url, data=data, headers=headers)
    try:
        response = urllib.request.urlopen(url=req,timeout=5)
    except:
        return "好像网不行~"
    html = response.read().decode('utf-8')
    res_map = json.loads(html)
    # pprint(res_map)
    res = text + '  ' + res_map['translation'][0]
    if 'basic' in res_map and res_map['basic'] != None:
        res +=  "\n美 /" + res_map['basic']['phonetic'] + '/ 英 /' + res_map['basic']['phonetic'] + "/\n" + '-'*10+'\n基本释义:'
        for i in res_map['basic']['explains']:
            res += '\n'+i

    if 'web' in res_map and res_map['web'] != None:
        res += '\n'+'-'*10+'\n网络释义:\n'
        for i in res_map['web'][0]['value']:
            res += i+' '
    return res+'\n'+'-'*10

# -------------------------------------------------------

def get_cmd_args():
    text_list = sys.argv[1:]
    text_len = len(text_list)
    if text_len == 0:
        return (0, '')
    elif text_len == 1:
        return (1, text_list[0])
    else:
        text = text_list[0]
        for i in range(1, text_len):
            text += ' '+text_list[i]
        return (text_len, text)


if __name__ == '__main__':
    cmdargs = get_cmd_args()
    text = ''
    if cmdargs[0] == 0:
        for i in sys.stdin:
            text+=i
    else:
        text = cmdargs[1]
    res = youdao(text)
    print(res)
