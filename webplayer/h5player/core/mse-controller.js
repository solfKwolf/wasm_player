
// import EventEmitter from '../util/events';
// import MSEEvents from './mse-events.js';

class MSEController {
    constructor() {

        this._emitter = new EventEmitter();

        this._mediaSource = null;
        this._mediaSourceObjectURL = null;
        this._mediaElement = null;

        this.e = {
            onSourceOpen: this._onSourceOpen.bind(this),
            onSourceEnded: this._onSourceEnded.bind(this),
            onSourceClose: this._onSourceClose.bind(this),
            onSourceBufferError: this._onSourceBufferError.bind(this),
            onSourceBufferUpdateEnd: this._onSourceBufferUpdateEnd.bind(this)
        };

        this._mimeType = null;
        this._pendingSegments = [];
        this._sourceBuffer = null;

        this._hasPendingEos = false;
    }

    destroy() {
        if (this._mediaElement || this._mediaSource) {
            this.detachMediaElement();
        }

        this.e = null;

        this._emitter.removeAllListeners();
        this._emitter = null;
    }

    on(event, listener) {
        this._emitter.addListener(event, listener)
    }

    off(event, listener) {
        this._emitter.removeAllListeners(event, listener)
    }

    attachMediaElement(mediaElement) {
        if (this._mediaSource) {
            throw new IllegalStateException('MediaSource has been attached to an HTMLMediaElement!');
        }
        let ms = this._mediaSource = new window.MediaSource();
        ms.addEventListener('sourceopen', this.e.onSourceOpen);
        ms.addEventListener('sourceended', this.e.onSourceEnded);
        ms.addEventListener('sourceclose', this.e.onSourceClose);

        this._mediaElement = mediaElement;
        this._mediaSourceObjectURL = window.URL.createObjectURL(this._mediaSource);
        mediaElement.src = this._mediaSourceObjectURL;
    }

    appendInitSegment(initSegment, mimeType) {
        if (!this._mediaSource || this._mediaSource.readyState != 'open') {
            // make sure that this InitSegment is in the front of pending segments queue
            this._pendingSegments.push(initSegment);
            this._mimeType = mimeType;
            return;
        }

        try {
            this._sourceBuffer = this._mediaSource.addSourceBuffer(mimeType)
            this._sourceBuffer.addEventListener('error', this.e.onSourceBufferError)
            this._sourceBuffer.addEventListener('updateend', this.e.onSourceBufferUpdateEnd)
        } catch(error) {
            console.log(error.message)
            this._emitter.emit(MSEEvents.ERROR, {code: error.code, msg: error.message});
            return;
        }

        this._mimeType = mimeType;
        this._doAppendToBuffer(initSegment)
        
    }

    appendMediaSegment(mediaSegment) {
        let ms = mediaSegment
        this._pendingSegments.push(ms)
        if (this._sourceBuffer && !this._sourceBuffer.updating) {
            this._doAppendSegments();
        }
    }

    endOfStream() {
        let ms = this._mediaSource;
        let sb = this._sourceBuffer;
        if (!ms || ms.readyState !== 'open') {
            if (ms && ms.readyState === 'closed' && this._hasPendingSegments()) {
                this._hasPendingEos = true;
            }
            return
        }
        if (sb.updating) {
            this._hasPendingEos = true;
        } else {
            this._hasPendingEos = false;
            ms.endOfStream();
        }
    }

    _onSourceOpen() {
        this._mediaSource.removeEventListener('sourceopen', this.e.onSourceOpen);

        if (this._hasPendingSegments()) {
            let segment = this._pendingSegments.shift();
            this.appendInitSegment(segment, this._mimeType);
        }

        if (this._hasPendingSegments()) {
            this._doAppendSegments();
        }

        this._emitter.emit(MSEEvents.SOURCE_OPEN);
    }

    _onSourceEnded() {
        // fired on endofStream
        console.log('MediaSource onSourceEnded');
    }

    _onSourceClose() {
        // fired on detaching from media element
        if (this._mediaSource && this.e != null) {
            this._mediaSource.removeEventListener('sourceopen', this.e.onSourceOpen);
            this._mediaSource.removeEventListener('sourceended', this.e.onSourceEnded);
            this._mediaSource.removeEventListener('sourceclose', this.e.onSourceClose);
        }
    }

    _onSourceBufferUpdateEnd() {
        if (this._hasPendingSegments()) {
            this._doAppendSegments()
        } else if (this._hasPendingEos) {
            this.endOfStream();
        }
    }

    _onSourceBufferError(e) {
        console.log(`SourceBuffer Error: `, e);
        console.log( this._mediaElement.error);
        // this error might not always be fatal, just ignore it
    }

    _doAppendSegments() {
        if (this._mediaSource.readyState == "closed") {
            console.log("media source closed")
            return;
        }
        if(this._pendingSegments.length > 0 && !this._sourceBuffer.updating) {
            let segment = this._pendingSegments.shift();
            this._doAppendToBuffer(segment);
        }
    }

    _doAppendToBuffer(segment) {
        try {
            this._sourceBuffer.appendBuffer(segment);
        } catch(error) {
            this._pendingSegments.unshift(segment);
            if (error.code === 22) {  // QuotaExceededError
                /* Notice that FireFox may not throw QuotaExceededError if SourceBuffer is full
                 * Currently we can only do lazy-load to avoid SourceBuffer become scattered.
                 * SourceBuffer eviction policy may be changed in future version of FireFox.
                 *
                 * Related issues:
                 * https://bugzilla.mozilla.org/show_bug.cgi?id=1279885
                 * https://bugzilla.mozilla.org/show_bug.cgi?id=1280023
                 */

                // report buffer full, abort network IO
                this._emitter.emit(MSEEvents.BUFFER_FULL);
            } else {
                this._emitter.emit(MSEEvents.ERROR, {code: error.code, msg: error.message});
            }
        }
    }

    _hasPendingSegments() {
        return this._pendingSegments.length > 0
    }
}

// export default MSEController;