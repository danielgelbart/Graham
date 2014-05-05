;;; Neat-o hardware: Emacs not only supports your vt100, but also your
;;; mouse wheel!
(require 'mwheel)
(mouse-wheel-mode 1)
;;; recentf: Menu of recently opened files
(require 'recentf)	    ; Also need to customize-group recentf and
					; turn it ON!


;; Make it easy to find "twin" file -- .h from .c/.cc/.cpp and vice-versa
(defcustom as-find-twin-patterns
  '(("\\(.*\\)\\.h$" "\\1.cpp" "\\1.cc" "\\1.c")
    ("\\(.*\\)\\.c$" "\\1.h")
    ("\\(.*\\)\\.cc$" "\\1.h")
    ("\\(.*\\)\\.cpp$" "\\1.h"))
  "Regular expression patterns for finding twin files
This is a list.  The first element of each list is a regexp, the rest
are all replacement patterns; if the current filename matches the
regexp, each of the replacement patterns is a candidate for the
filename of this file's twin."
  :type '(repeat
	  (cons (regexp :tag "If filename matches")
		(repeat (string :tag "then its twin might be")))))

(defun as-find-twin-file nil
  "Find this file's twin: matching .h, etc.
The twins are defined in `as-find-twin-patterns'.  The first matching twin
will be visited."
  (interactive)
  (let ((twins as-find-twin-patterns) twin-filename)
    (while (and twins (not twin-filename))
      (let ((fregexp (caar twins))
	    (fpatterns (cdar twins)))
	(if (string-match fregexp buffer-file-name)
	    (while (and fpatterns (not twin-filename))
	      (let ((fname
		     (replace-match (car fpatterns) nil nil buffer-file-name)))
		(if (or (file-exists-p fname)
			(stringp (file-symlink-p fname)))
		    (setq twin-filename fname)))
	      (setq fpatterns (cdr fpatterns)))))
      (setq twins (cdr twins)))
    (if twin-filename
	(find-file twin-filename))))

(global-set-key (kbd "C-c f") 'as-find-twin-file)

;;; automagically recognize make.* and *.mak as makefiles
(setq auto-mode-alist
      (append
       '(("make\\(file\\)?\\.[a-zA-Z0-9]*$" . makefile-mode)
	 ("\\.mak$" . makefile-mode)
	 ("\\.d$" . makefile-mode))
       auto-mode-alist))

;; C++ style: don't indent namespaces
(c-add-style "ariels_style"
	     '("user" (c-offsets-alist (innamespace . 0) (substatement-open . 0))))


;; C/C++: Setup a default compile command where possible
(defcustom as-default-path-for-compile-command nil
  "List of regexps matching default paths
When you visit any file in `c-mode' or `c++-mode', if its pathname matches
a regexp, then the matching portion of the path is used as a directory name
for `compile-command'."
  :group 'compilation
  :type '(repeat (regexp :tag "Directory matching")))

(defun as-setup-compile-command nil
  "Set up a good default `compile-command'.
This is a good function to put inside `c-mode-hook', `c++-mode-hook', or
possibly `c-mode-common-hook'."
  (interactive)
  (if buffer-file-name			;not an error if not -- or we
					;can't set c{,++}-mode on a
					;buffer with no file!
      (let ((cc-path as-default-path-for-compile-command)
	    (new-cc-dir nil))
	(while (and cc-path (not new-cc-dir))
	  (if (string-match (expand-file-name (car cc-path)) buffer-file-name)
	      (setq new-cc-dir (match-string 0 buffer-file-name)))
	  (setq cc-path (cdr cc-path)))
	(when new-cc-dir
	  (set (make-local-variable 'compile-command)
	       (format "make -C %s -k -j4" new-cc-dir))
          ;;(message "`compile-command' set to %s" compile-command)
	  ))))

(add-hook 'makefile-mode-hook 'as-setup-compile-command)
(add-hook 'c-mode-common-hook 'as-setup-compile-command)

(add-hook 'c-mode-common-hook 'c-toggle-hungry-state)


(global-set-key (kbd "C-c g") 'goto-line)


(custom-set-variables
  ;; custom-set-variables was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(TeX-output-view-style (quote (("^dvi$" ("^landscape$" "^pstricks$\\|^pst-\\|^psfrag$") "%(o?)dvips -t landscape %d -o && gv %f") ("^dvi$" "^pstricks$\\|^pst-\\|^psfrag$" "%(o?)dvips %d -o && gv %f") ("^dvi$" ("^a4\\(?:dutch\\|paper\\|wide\\)\\|sem-a4$" "^landscape$") "%(o?)xdvi %dS -paper a4r -s 0 %d") ("^dvi$" "^a4\\(?:dutch\\|paper\\|wide\\)\\|sem-a4$" "%(o?)xdvi %dS -paper a4 %d") ("^dvi$" ("^a5\\(?:comb\\|paper\\)$" "^landscape$") "%(o?)xdvi %dS -paper a5r -s 0 %d") ("^dvi$" "^a5\\(?:comb\\|paper\\)$" "%(o?)xdvi %dS -paper a5 %d") ("^dvi$" "^b5paper$" "%(o?)xdvi %dS -paper b5 %d") ("^dvi$" "^letterpaper$" "%(o?)xdvi %dS -paper us %d") ("^dvi$" "^legalpaper$" "%(o?)xdvi %dS -paper legal %d") ("^dvi$" "^executivepaper$" "%(o?)xdvi %dS -paper 7.25x10.5in %d") ("^dvi$" "." "%(o?)xdvi %dS %d") ("^pdf$" "." "evince %o %(outpage)") ("^html?$" "." "netscape %o"))))
 '(TeX-view-program-selection (quote (((output-dvi style-pstricks) "dvips and gv") (output-dvi "xdvi") (output-pdf "Evince") (output-html "xdg-open"))))
 '(as-default-path-for-compile-command (quote ("~/dev/[^/]+/src/")))
 '(c-basic-offset 4)
 '(c-default-style (quote ((c++-mode . "ariels_style") (java-mode . "java") (other . "gnu"))))
 '(c-tab-always-indent nil)
 '(case-fold-search t)
 '(column-number-mode t)
 '(compilation-window-height 15)
 '(current-language-environment "UTF-8")
 '(default-input-method "rfc1345")
 '(footprints-mode-hook (quote (turn-on-font-lock-if-enabled turn-on-auto-revert-mode toggle-truncate-lines)))
 '(frame-background-mode (quote dark))
 '(global-font-lock-mode t nil (font-lock))
 '(indent-tabs-mode nil)
 '(mouse-wheel-mode t nil (mwheel))
 '(pc-selection-mode t nil (pc-select))
 '(recentf-mode t nil (recentf))
 '(show-paren-mode t nil (paren))
 '(svn-status-verbose nil)
 '(tab-always-indent t)
 '(text-mode-hook (quote (turn-on-auto-fill text-mode-hook-identify)))
 '(uniquify-buffer-name-style (quote forward) nil (uniquify))
 '(vc-handled-backends (quote (RCS CVS SVN SCCS Bzr Hg Mtn Arch))))
(custom-set-faces
  ;; custom-set-faces was added by Custom.
  ;; If you edit it by hand, you could mess it up, so be careful.
  ;; Your init file should contain only one such instance.
  ;; If there is more than one, they won't work right.
 '(default ((t (:stipple nil :background "black" :foreground "white" :inverse-video nil :box nil :strike-through nil :overline nil :underline nil :slant normal :weight normal :height 120 :width normal :foundry "unknown" :family "DejaVu Sans Mono"))))
 '(as-footprint-token ((t (:background "blue")))))




(setq auto-mode-alist (cons '("\\.h$" . c++-mode) auto-mode-alist))
(setq auto-mode-alist (cons '("\\.hpp.erb$" . c++-mode) auto-mode-alist))
(setq auto-mode-alist (cons '("\\.cpp.erb$" . c++-mode) auto-mode-alist))
(global-set-key "\M-g" 'goto-line)
(global-set-key "\M-c" 'compile)

;; (put 'upcase-region 'disabled nil)


(setq load-path (cons (expand-file-name "~/elisp/") load-path))
(setq load-path (cons (expand-file-name "~/.emacs.d/") load-path))

(setq default-frame-alist
 '(
; frame width and height
    (width             . 80)
    (height            . 45)
    (top               . 0)
    (left              . 680)
  )
)

(ido-mode)

;;cscope integration
;;(require 'xcscope)

(set-cursor-color "white")

;(add-to-list 'load-path "/home/gelbart/git-emacs-1.1")
;(require 'git-emacs)

(put 'downcase-region 'disabled nil)

(autoload 'flyspell-mode "flyspell" "On-the-fly spelling checker." t)

(add-hook 'LaTeX-mode-hook
      (lambda ()
        (setq TeX-auto-save t)
        (setq TeX-parse-self t)
        (setq TeX-newline-function 'newline-and-indent)
        (reftex-mode t)
	(auto-fill-mode t)
	(tex-pdf-mode)
        (LaTeX-math-mode)
        (TeX-fold-mode t)
        (flyspell-mode)))

;(load "preview-latex.el" nil t t)



;; open speedbar on init
;; (when window-system (speedbar t))

;; define key 'F1' to toggle frames to switch to speedbar 
(global-set-key "f1" 'other-frame')


(add-to-list 'load-path "/home/gelbart/.emacs.d/")


;; FOR SOME REASON LINES BELOW DO NOT RUN/WORK

;; Load CEDET.
;; See cedet/common/cedet.info for configuration details.
;; IMPORTANT: For Emacs >= 23.2, you must place this *before* any
;; CEDET component (including EIEIO) gets activated by another 
;; package (Gnus, auth-source, ...).
(load-file "~/cedet-VERSION/common/cedet.el")

;; Enable EDE (Project Management) features
(global-ede-mode 1)

;; Enable EDE for a pre-existing C++ project
;; (ede-cpp-root-project "NAME" :file "~/myproject/Makefile")


;; Enabling Semantic (code-parsing, smart completion) features
;; Select one of the following:

;; * This enables the database and idle reparse engines
(semantic-load-enable-minimum-features)

;; * This enables some tools useful for coding, such as summary mode,
;;   imenu support, and the semantic navigator
(semantic-load-enable-code-helpers)

;; * This enables even more coding tools such as intellisense mode,
;;   decoration mode, and stickyfunc mode (plus regular code helpers)
;; (semantic-load-enable-gaudy-code-helpers)


;; Enable SRecode (Template management) minor-mode.
;; (global-srecode-minor-mode 1)
