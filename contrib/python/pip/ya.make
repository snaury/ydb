# Generated by devtools/yamaker (pypi).

PY3_LIBRARY()

VERSION(24.0)

LICENSE(MIT)

NO_LINT()

NO_CHECK_IMPORTS(
    pip.__pip-runner__
    pip._vendor.*
)

PY_SRCS(
    TOP_LEVEL
    pip/__init__.py
    pip/__main__.py
    pip/__pip-runner__.py
    pip/_internal/__init__.py
    pip/_internal/build_env.py
    pip/_internal/cache.py
    pip/_internal/cli/__init__.py
    pip/_internal/cli/autocompletion.py
    pip/_internal/cli/base_command.py
    pip/_internal/cli/cmdoptions.py
    pip/_internal/cli/command_context.py
    pip/_internal/cli/main.py
    pip/_internal/cli/main_parser.py
    pip/_internal/cli/parser.py
    pip/_internal/cli/progress_bars.py
    pip/_internal/cli/req_command.py
    pip/_internal/cli/spinners.py
    pip/_internal/cli/status_codes.py
    pip/_internal/commands/__init__.py
    pip/_internal/commands/cache.py
    pip/_internal/commands/check.py
    pip/_internal/commands/completion.py
    pip/_internal/commands/configuration.py
    pip/_internal/commands/debug.py
    pip/_internal/commands/download.py
    pip/_internal/commands/freeze.py
    pip/_internal/commands/hash.py
    pip/_internal/commands/help.py
    pip/_internal/commands/index.py
    pip/_internal/commands/inspect.py
    pip/_internal/commands/install.py
    pip/_internal/commands/list.py
    pip/_internal/commands/search.py
    pip/_internal/commands/show.py
    pip/_internal/commands/uninstall.py
    pip/_internal/commands/wheel.py
    pip/_internal/configuration.py
    pip/_internal/distributions/__init__.py
    pip/_internal/distributions/base.py
    pip/_internal/distributions/installed.py
    pip/_internal/distributions/sdist.py
    pip/_internal/distributions/wheel.py
    pip/_internal/exceptions.py
    pip/_internal/index/__init__.py
    pip/_internal/index/collector.py
    pip/_internal/index/package_finder.py
    pip/_internal/index/sources.py
    pip/_internal/locations/__init__.py
    pip/_internal/locations/_distutils.py
    pip/_internal/locations/_sysconfig.py
    pip/_internal/locations/base.py
    pip/_internal/main.py
    pip/_internal/metadata/__init__.py
    pip/_internal/metadata/_json.py
    pip/_internal/metadata/base.py
    pip/_internal/metadata/importlib/__init__.py
    pip/_internal/metadata/importlib/_compat.py
    pip/_internal/metadata/importlib/_dists.py
    pip/_internal/metadata/importlib/_envs.py
    pip/_internal/metadata/pkg_resources.py
    pip/_internal/models/__init__.py
    pip/_internal/models/candidate.py
    pip/_internal/models/direct_url.py
    pip/_internal/models/format_control.py
    pip/_internal/models/index.py
    pip/_internal/models/installation_report.py
    pip/_internal/models/link.py
    pip/_internal/models/scheme.py
    pip/_internal/models/search_scope.py
    pip/_internal/models/selection_prefs.py
    pip/_internal/models/target_python.py
    pip/_internal/models/wheel.py
    pip/_internal/network/__init__.py
    pip/_internal/network/auth.py
    pip/_internal/network/cache.py
    pip/_internal/network/download.py
    pip/_internal/network/lazy_wheel.py
    pip/_internal/network/session.py
    pip/_internal/network/utils.py
    pip/_internal/network/xmlrpc.py
    pip/_internal/operations/__init__.py
    pip/_internal/operations/build/__init__.py
    pip/_internal/operations/build/build_tracker.py
    pip/_internal/operations/build/metadata.py
    pip/_internal/operations/build/metadata_editable.py
    pip/_internal/operations/build/metadata_legacy.py
    pip/_internal/operations/build/wheel.py
    pip/_internal/operations/build/wheel_editable.py
    pip/_internal/operations/build/wheel_legacy.py
    pip/_internal/operations/check.py
    pip/_internal/operations/freeze.py
    pip/_internal/operations/install/__init__.py
    pip/_internal/operations/install/editable_legacy.py
    pip/_internal/operations/install/wheel.py
    pip/_internal/operations/prepare.py
    pip/_internal/pyproject.py
    pip/_internal/req/__init__.py
    pip/_internal/req/constructors.py
    pip/_internal/req/req_file.py
    pip/_internal/req/req_install.py
    pip/_internal/req/req_set.py
    pip/_internal/req/req_uninstall.py
    pip/_internal/resolution/__init__.py
    pip/_internal/resolution/base.py
    pip/_internal/resolution/legacy/__init__.py
    pip/_internal/resolution/legacy/resolver.py
    pip/_internal/resolution/resolvelib/__init__.py
    pip/_internal/resolution/resolvelib/base.py
    pip/_internal/resolution/resolvelib/candidates.py
    pip/_internal/resolution/resolvelib/factory.py
    pip/_internal/resolution/resolvelib/found_candidates.py
    pip/_internal/resolution/resolvelib/provider.py
    pip/_internal/resolution/resolvelib/reporter.py
    pip/_internal/resolution/resolvelib/requirements.py
    pip/_internal/resolution/resolvelib/resolver.py
    pip/_internal/self_outdated_check.py
    pip/_internal/utils/__init__.py
    pip/_internal/utils/_jaraco_text.py
    pip/_internal/utils/_log.py
    pip/_internal/utils/appdirs.py
    pip/_internal/utils/compat.py
    pip/_internal/utils/compatibility_tags.py
    pip/_internal/utils/datetime.py
    pip/_internal/utils/deprecation.py
    pip/_internal/utils/direct_url_helpers.py
    pip/_internal/utils/egg_link.py
    pip/_internal/utils/encoding.py
    pip/_internal/utils/entrypoints.py
    pip/_internal/utils/filesystem.py
    pip/_internal/utils/filetypes.py
    pip/_internal/utils/glibc.py
    pip/_internal/utils/hashes.py
    pip/_internal/utils/logging.py
    pip/_internal/utils/misc.py
    pip/_internal/utils/models.py
    pip/_internal/utils/packaging.py
    pip/_internal/utils/setuptools_build.py
    pip/_internal/utils/subprocess.py
    pip/_internal/utils/temp_dir.py
    pip/_internal/utils/unpacking.py
    pip/_internal/utils/urls.py
    pip/_internal/utils/virtualenv.py
    pip/_internal/utils/wheel.py
    pip/_internal/vcs/__init__.py
    pip/_internal/vcs/bazaar.py
    pip/_internal/vcs/git.py
    pip/_internal/vcs/mercurial.py
    pip/_internal/vcs/subversion.py
    pip/_internal/vcs/versioncontrol.py
    pip/_internal/wheel_builder.py
    pip/_vendor/__init__.py
    pip/_vendor/cachecontrol/__init__.py
    pip/_vendor/cachecontrol/_cmd.py
    pip/_vendor/cachecontrol/adapter.py
    pip/_vendor/cachecontrol/cache.py
    pip/_vendor/cachecontrol/caches/__init__.py
    pip/_vendor/cachecontrol/caches/file_cache.py
    pip/_vendor/cachecontrol/caches/redis_cache.py
    pip/_vendor/cachecontrol/controller.py
    pip/_vendor/cachecontrol/filewrapper.py
    pip/_vendor/cachecontrol/heuristics.py
    pip/_vendor/cachecontrol/serialize.py
    pip/_vendor/cachecontrol/wrapper.py
    pip/_vendor/certifi/__init__.py
    pip/_vendor/certifi/__main__.py
    pip/_vendor/certifi/core.py
    pip/_vendor/chardet/__init__.py
    pip/_vendor/chardet/big5freq.py
    pip/_vendor/chardet/big5prober.py
    pip/_vendor/chardet/chardistribution.py
    pip/_vendor/chardet/charsetgroupprober.py
    pip/_vendor/chardet/charsetprober.py
    pip/_vendor/chardet/cli/__init__.py
    pip/_vendor/chardet/cli/chardetect.py
    pip/_vendor/chardet/codingstatemachine.py
    pip/_vendor/chardet/codingstatemachinedict.py
    pip/_vendor/chardet/cp949prober.py
    pip/_vendor/chardet/enums.py
    pip/_vendor/chardet/escprober.py
    pip/_vendor/chardet/escsm.py
    pip/_vendor/chardet/eucjpprober.py
    pip/_vendor/chardet/euckrfreq.py
    pip/_vendor/chardet/euckrprober.py
    pip/_vendor/chardet/euctwfreq.py
    pip/_vendor/chardet/euctwprober.py
    pip/_vendor/chardet/gb2312freq.py
    pip/_vendor/chardet/gb2312prober.py
    pip/_vendor/chardet/hebrewprober.py
    pip/_vendor/chardet/jisfreq.py
    pip/_vendor/chardet/johabfreq.py
    pip/_vendor/chardet/johabprober.py
    pip/_vendor/chardet/jpcntx.py
    pip/_vendor/chardet/langbulgarianmodel.py
    pip/_vendor/chardet/langgreekmodel.py
    pip/_vendor/chardet/langhebrewmodel.py
    pip/_vendor/chardet/langhungarianmodel.py
    pip/_vendor/chardet/langrussianmodel.py
    pip/_vendor/chardet/langthaimodel.py
    pip/_vendor/chardet/langturkishmodel.py
    pip/_vendor/chardet/latin1prober.py
    pip/_vendor/chardet/macromanprober.py
    pip/_vendor/chardet/mbcharsetprober.py
    pip/_vendor/chardet/mbcsgroupprober.py
    pip/_vendor/chardet/mbcssm.py
    pip/_vendor/chardet/metadata/__init__.py
    pip/_vendor/chardet/metadata/languages.py
    pip/_vendor/chardet/resultdict.py
    pip/_vendor/chardet/sbcharsetprober.py
    pip/_vendor/chardet/sbcsgroupprober.py
    pip/_vendor/chardet/sjisprober.py
    pip/_vendor/chardet/universaldetector.py
    pip/_vendor/chardet/utf1632prober.py
    pip/_vendor/chardet/utf8prober.py
    pip/_vendor/chardet/version.py
    pip/_vendor/colorama/__init__.py
    pip/_vendor/colorama/ansi.py
    pip/_vendor/colorama/ansitowin32.py
    pip/_vendor/colorama/initialise.py
    pip/_vendor/colorama/win32.py
    pip/_vendor/colorama/winterm.py
    pip/_vendor/distlib/__init__.py
    pip/_vendor/distlib/compat.py
    pip/_vendor/distlib/database.py
    pip/_vendor/distlib/index.py
    pip/_vendor/distlib/locators.py
    pip/_vendor/distlib/manifest.py
    pip/_vendor/distlib/markers.py
    pip/_vendor/distlib/metadata.py
    pip/_vendor/distlib/resources.py
    pip/_vendor/distlib/scripts.py
    pip/_vendor/distlib/util.py
    pip/_vendor/distlib/version.py
    pip/_vendor/distlib/wheel.py
    pip/_vendor/distro/__init__.py
    pip/_vendor/distro/__main__.py
    pip/_vendor/distro/distro.py
    pip/_vendor/idna/__init__.py
    pip/_vendor/idna/codec.py
    pip/_vendor/idna/compat.py
    pip/_vendor/idna/core.py
    pip/_vendor/idna/idnadata.py
    pip/_vendor/idna/intranges.py
    pip/_vendor/idna/package_data.py
    pip/_vendor/idna/uts46data.py
    pip/_vendor/msgpack/__init__.py
    pip/_vendor/msgpack/exceptions.py
    pip/_vendor/msgpack/ext.py
    pip/_vendor/msgpack/fallback.py
    pip/_vendor/packaging/__about__.py
    pip/_vendor/packaging/__init__.py
    pip/_vendor/packaging/_manylinux.py
    pip/_vendor/packaging/_musllinux.py
    pip/_vendor/packaging/_structures.py
    pip/_vendor/packaging/markers.py
    pip/_vendor/packaging/requirements.py
    pip/_vendor/packaging/specifiers.py
    pip/_vendor/packaging/tags.py
    pip/_vendor/packaging/utils.py
    pip/_vendor/packaging/version.py
    pip/_vendor/pkg_resources/__init__.py
    pip/_vendor/platformdirs/__init__.py
    pip/_vendor/platformdirs/__main__.py
    pip/_vendor/platformdirs/android.py
    pip/_vendor/platformdirs/api.py
    pip/_vendor/platformdirs/macos.py
    pip/_vendor/platformdirs/unix.py
    pip/_vendor/platformdirs/version.py
    pip/_vendor/platformdirs/windows.py
    pip/_vendor/pygments/__init__.py
    pip/_vendor/pygments/__main__.py
    pip/_vendor/pygments/cmdline.py
    pip/_vendor/pygments/console.py
    pip/_vendor/pygments/filter.py
    pip/_vendor/pygments/filters/__init__.py
    pip/_vendor/pygments/formatter.py
    pip/_vendor/pygments/formatters/__init__.py
    pip/_vendor/pygments/formatters/_mapping.py
    pip/_vendor/pygments/formatters/bbcode.py
    pip/_vendor/pygments/formatters/groff.py
    pip/_vendor/pygments/formatters/html.py
    pip/_vendor/pygments/formatters/img.py
    pip/_vendor/pygments/formatters/irc.py
    pip/_vendor/pygments/formatters/latex.py
    pip/_vendor/pygments/formatters/other.py
    pip/_vendor/pygments/formatters/pangomarkup.py
    pip/_vendor/pygments/formatters/rtf.py
    pip/_vendor/pygments/formatters/svg.py
    pip/_vendor/pygments/formatters/terminal.py
    pip/_vendor/pygments/formatters/terminal256.py
    pip/_vendor/pygments/lexer.py
    pip/_vendor/pygments/lexers/__init__.py
    pip/_vendor/pygments/lexers/_mapping.py
    pip/_vendor/pygments/lexers/python.py
    pip/_vendor/pygments/modeline.py
    pip/_vendor/pygments/plugin.py
    pip/_vendor/pygments/regexopt.py
    pip/_vendor/pygments/scanner.py
    pip/_vendor/pygments/sphinxext.py
    pip/_vendor/pygments/style.py
    pip/_vendor/pygments/styles/__init__.py
    pip/_vendor/pygments/token.py
    pip/_vendor/pygments/unistring.py
    pip/_vendor/pygments/util.py
    pip/_vendor/pyparsing/__init__.py
    pip/_vendor/pyparsing/actions.py
    pip/_vendor/pyparsing/common.py
    pip/_vendor/pyparsing/core.py
    pip/_vendor/pyparsing/diagram/__init__.py
    pip/_vendor/pyparsing/exceptions.py
    pip/_vendor/pyparsing/helpers.py
    pip/_vendor/pyparsing/results.py
    pip/_vendor/pyparsing/testing.py
    pip/_vendor/pyparsing/unicode.py
    pip/_vendor/pyparsing/util.py
    pip/_vendor/pyproject_hooks/__init__.py
    pip/_vendor/pyproject_hooks/_compat.py
    pip/_vendor/pyproject_hooks/_impl.py
    pip/_vendor/pyproject_hooks/_in_process/__init__.py
    pip/_vendor/pyproject_hooks/_in_process/_in_process.py
    pip/_vendor/requests/__init__.py
    pip/_vendor/requests/__version__.py
    pip/_vendor/requests/_internal_utils.py
    pip/_vendor/requests/adapters.py
    pip/_vendor/requests/api.py
    pip/_vendor/requests/auth.py
    pip/_vendor/requests/certs.py
    pip/_vendor/requests/compat.py
    pip/_vendor/requests/cookies.py
    pip/_vendor/requests/exceptions.py
    pip/_vendor/requests/help.py
    pip/_vendor/requests/hooks.py
    pip/_vendor/requests/models.py
    pip/_vendor/requests/packages.py
    pip/_vendor/requests/sessions.py
    pip/_vendor/requests/status_codes.py
    pip/_vendor/requests/structures.py
    pip/_vendor/requests/utils.py
    pip/_vendor/resolvelib/__init__.py
    pip/_vendor/resolvelib/compat/__init__.py
    pip/_vendor/resolvelib/compat/collections_abc.py
    pip/_vendor/resolvelib/providers.py
    pip/_vendor/resolvelib/reporters.py
    pip/_vendor/resolvelib/resolvers.py
    pip/_vendor/resolvelib/structs.py
    pip/_vendor/rich/__init__.py
    pip/_vendor/rich/__main__.py
    pip/_vendor/rich/_cell_widths.py
    pip/_vendor/rich/_emoji_codes.py
    pip/_vendor/rich/_emoji_replace.py
    pip/_vendor/rich/_export_format.py
    pip/_vendor/rich/_extension.py
    pip/_vendor/rich/_fileno.py
    pip/_vendor/rich/_inspect.py
    pip/_vendor/rich/_log_render.py
    pip/_vendor/rich/_loop.py
    pip/_vendor/rich/_null_file.py
    pip/_vendor/rich/_palettes.py
    pip/_vendor/rich/_pick.py
    pip/_vendor/rich/_ratio.py
    pip/_vendor/rich/_spinners.py
    pip/_vendor/rich/_stack.py
    pip/_vendor/rich/_timer.py
    pip/_vendor/rich/_win32_console.py
    pip/_vendor/rich/_windows.py
    pip/_vendor/rich/_windows_renderer.py
    pip/_vendor/rich/_wrap.py
    pip/_vendor/rich/abc.py
    pip/_vendor/rich/align.py
    pip/_vendor/rich/ansi.py
    pip/_vendor/rich/bar.py
    pip/_vendor/rich/box.py
    pip/_vendor/rich/cells.py
    pip/_vendor/rich/color.py
    pip/_vendor/rich/color_triplet.py
    pip/_vendor/rich/columns.py
    pip/_vendor/rich/console.py
    pip/_vendor/rich/constrain.py
    pip/_vendor/rich/containers.py
    pip/_vendor/rich/control.py
    pip/_vendor/rich/default_styles.py
    pip/_vendor/rich/diagnose.py
    pip/_vendor/rich/emoji.py
    pip/_vendor/rich/errors.py
    pip/_vendor/rich/file_proxy.py
    pip/_vendor/rich/filesize.py
    pip/_vendor/rich/highlighter.py
    pip/_vendor/rich/json.py
    pip/_vendor/rich/jupyter.py
    pip/_vendor/rich/layout.py
    pip/_vendor/rich/live.py
    pip/_vendor/rich/live_render.py
    pip/_vendor/rich/logging.py
    pip/_vendor/rich/markup.py
    pip/_vendor/rich/measure.py
    pip/_vendor/rich/padding.py
    pip/_vendor/rich/pager.py
    pip/_vendor/rich/palette.py
    pip/_vendor/rich/panel.py
    pip/_vendor/rich/pretty.py
    pip/_vendor/rich/progress.py
    pip/_vendor/rich/progress_bar.py
    pip/_vendor/rich/prompt.py
    pip/_vendor/rich/protocol.py
    pip/_vendor/rich/region.py
    pip/_vendor/rich/repr.py
    pip/_vendor/rich/rule.py
    pip/_vendor/rich/scope.py
    pip/_vendor/rich/screen.py
    pip/_vendor/rich/segment.py
    pip/_vendor/rich/spinner.py
    pip/_vendor/rich/status.py
    pip/_vendor/rich/style.py
    pip/_vendor/rich/styled.py
    pip/_vendor/rich/syntax.py
    pip/_vendor/rich/table.py
    pip/_vendor/rich/terminal_theme.py
    pip/_vendor/rich/text.py
    pip/_vendor/rich/theme.py
    pip/_vendor/rich/themes.py
    pip/_vendor/rich/traceback.py
    pip/_vendor/rich/tree.py
    pip/_vendor/six.py
    pip/_vendor/tenacity/__init__.py
    pip/_vendor/tenacity/_asyncio.py
    pip/_vendor/tenacity/_utils.py
    pip/_vendor/tenacity/after.py
    pip/_vendor/tenacity/before.py
    pip/_vendor/tenacity/before_sleep.py
    pip/_vendor/tenacity/nap.py
    pip/_vendor/tenacity/retry.py
    pip/_vendor/tenacity/stop.py
    pip/_vendor/tenacity/tornadoweb.py
    pip/_vendor/tenacity/wait.py
    pip/_vendor/tomli/__init__.py
    pip/_vendor/tomli/_parser.py
    pip/_vendor/tomli/_re.py
    pip/_vendor/tomli/_types.py
    pip/_vendor/truststore/__init__.py
    pip/_vendor/truststore/_api.py
    pip/_vendor/truststore/_macos.py
    pip/_vendor/truststore/_openssl.py
    pip/_vendor/truststore/_ssl_constants.py
    pip/_vendor/truststore/_windows.py
    pip/_vendor/typing_extensions.py
    pip/_vendor/urllib3/__init__.py
    pip/_vendor/urllib3/_collections.py
    pip/_vendor/urllib3/_version.py
    pip/_vendor/urllib3/connection.py
    pip/_vendor/urllib3/connectionpool.py
    pip/_vendor/urllib3/contrib/__init__.py
    pip/_vendor/urllib3/contrib/_appengine_environ.py
    pip/_vendor/urllib3/contrib/_securetransport/__init__.py
    pip/_vendor/urllib3/contrib/_securetransport/bindings.py
    pip/_vendor/urllib3/contrib/_securetransport/low_level.py
    pip/_vendor/urllib3/contrib/appengine.py
    pip/_vendor/urllib3/contrib/ntlmpool.py
    pip/_vendor/urllib3/contrib/pyopenssl.py
    pip/_vendor/urllib3/contrib/securetransport.py
    pip/_vendor/urllib3/contrib/socks.py
    pip/_vendor/urllib3/exceptions.py
    pip/_vendor/urllib3/fields.py
    pip/_vendor/urllib3/filepost.py
    pip/_vendor/urllib3/packages/__init__.py
    pip/_vendor/urllib3/packages/backports/__init__.py
    pip/_vendor/urllib3/packages/backports/makefile.py
    pip/_vendor/urllib3/packages/backports/weakref_finalize.py
    pip/_vendor/urllib3/packages/six.py
    pip/_vendor/urllib3/poolmanager.py
    pip/_vendor/urllib3/request.py
    pip/_vendor/urllib3/response.py
    pip/_vendor/urllib3/util/__init__.py
    pip/_vendor/urllib3/util/connection.py
    pip/_vendor/urllib3/util/proxy.py
    pip/_vendor/urllib3/util/queue.py
    pip/_vendor/urllib3/util/request.py
    pip/_vendor/urllib3/util/response.py
    pip/_vendor/urllib3/util/retry.py
    pip/_vendor/urllib3/util/ssl_.py
    pip/_vendor/urllib3/util/ssl_match_hostname.py
    pip/_vendor/urllib3/util/ssltransport.py
    pip/_vendor/urllib3/util/timeout.py
    pip/_vendor/urllib3/util/url.py
    pip/_vendor/urllib3/util/wait.py
    pip/_vendor/webencodings/__init__.py
    pip/_vendor/webencodings/labels.py
    pip/_vendor/webencodings/mklabels.py
    pip/_vendor/webencodings/x_user_defined.py
)

RESOURCE_FILES(
    PREFIX contrib/python/pip/
    .dist-info/METADATA
    .dist-info/entry_points.txt
    .dist-info/top_level.txt
    pip/_vendor/cachecontrol/py.typed
    pip/_vendor/certifi/cacert.pem
    pip/_vendor/certifi/py.typed
    pip/_vendor/chardet/py.typed
    pip/_vendor/distro/py.typed
    pip/_vendor/idna/py.typed
    pip/_vendor/packaging/py.typed
    pip/_vendor/platformdirs/py.typed
    pip/_vendor/pyparsing/py.typed
    pip/_vendor/resolvelib/py.typed
    pip/_vendor/rich/py.typed
    pip/_vendor/tenacity/py.typed
    pip/_vendor/tomli/py.typed
    pip/_vendor/truststore/py.typed
    pip/_vendor/vendor.txt
    pip/py.typed
)

END()
