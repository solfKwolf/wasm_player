
const AuthenticatorType = {
    AuthenticatorTypeNone : 0,
	AuthenticatorTypeDigest : 1,
	AuthenticatorTypeBasic : 2
}

export class Authenticator{
    constructor(){
        this.realm = "";
        this.nonce = "";
        this.username = "";
        this.password = "";
        this.authenicatorType = AuthenticatorType.AuthenticatorTypeNone;         
    }

    newAuthenticator(){
        this.authenicatorType = AuthenticatorType.AuthenticatorTypeNone;
    }

    md5StringToString(src){
        return hex_md5(src)
    }

    createAuthenticatorString(cmd, url){
        let authenticatorString;
        if (this.authenicatorType == AuthenticatorType.AuthenticatorTypeBasic){
            let strResponse = Base64.encode(authenticator.username + ":" + authenticator.password)
            authenticatorString = "Authorization: Basic " + strResponse + "\r\n";
        }
        else if (this.authenicatorType == AuthenticatorType.AuthenticatorTypeDigest){
            let strResponse = this.md5StringToString(this.md5StringToString(this.username + ":" + this.realm + ":" + this.password) + ":" + this.nonce + ":" + this.md5StringToString(cmd + ":" + url));
            authenticatorString = "Authorization: Digest username=" + "\"" + this.username + "\"" + ", realm=" + "\"" + this.realm + "\"" + ", nonce=" + "\"" + this.nonce + "\"" + ", uri=" + "\"" + url + "\"" + ", response=" + "\"" + strResponse + "\"" +"\r\n";
        }
        return authenticatorString;
    }
}
