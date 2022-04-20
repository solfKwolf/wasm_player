class XHRLoader extends BaseLoader {
    constructor() {
        super();
        this._xhr = null;
    }

    destroy() {
        if (this.isWorking()) {
            this.abort();
        }

        if (this._xhr) {
            this._xhr.onreadystatechange = null;
            this._xhr.onprogress = null;
            this._xhr.onload = null;
            this._xhr.onerror = null;
            this._xhr = null;
        }

        super.destroy();
    }

    open(dataSource) {
        this._status = LoaderStatus.kConnecting;
        this._xhr = new XMLHttpRequest();
        this._xhr.open('GET', dataSource, true);
        this._xhr.responseType = 'arraybuffer';
        this._xhr.onreadystatechange = this._onReadyStateChange.bind(this);
        this._xhr.onprogress = this._onProgress.bind(this);
        this._xhr.onerror = this._onXhrError.bind(this);
        this._xhr.onload = this._onLoad.bind(this);

        this._xhr.send();
    }

    abort() {
        this._internalAbort();
        this._status = LoaderStatus.kComplete;
    }

    _internalAbort() {
        if (this._xhr) {
            this._xhr.onreadystatechange = null;
            this._xhr.onprogress = null;
            this._xhr.onload = null;
            this._xhr.onerror = null;
            this._xhr.abort();
            this._xhr = null;
        }
    }

    _onReadyStateChange(e) {
        let xhr = e.target;

        if (xhr.readState === 2) {  // HEADERS_RECEIVED
            if (xhr.status >= 200 && xhr.status <= 299) {
                this._status = LoaderStatus.kBuffering;
            } else {
                this._status = LoaderStatus.kError;
                if (this._onError) {
                    this._onError(LoaderErrors.HTTP_STATUS_CODE_INVALID, {code: xhr.status, msg: xhr.statusText});
                } else {
                    throw new RuntimeException('XHRLoader: Http code invalid, ' + xhr.status + ' ' + xhr.statusText);
                }
            }
        }
    }

    _onProgress(e) {
        if (this._status === LoaderStatus.kError) {
            return;
        }
    }

    _onLoad(e) {
        if (this._status === LoaderStatus.kError) {
            return;
        }
        let buffer = e.target.response;
        if (this._onDataArrival) {
            this._onDataArrival(buffer);
        }

        if (this._onComplete) {
            this._onComplete();
        }
    }

    _onXhrError(e) {
        if (this.onError) {
            this._onError({code: LoaderErrors.EXCEPTION, msg: e.constructor.name + ' ' + e.type});
        }
    }
}