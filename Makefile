.PHONY: help
help:
	@echo "Makefile Help Menu"
	@echo "------------------"
	@echo "test: Run all tests"
	@echo "help: Print this message"

.PHONY: FORCE
FORCE:

.PHONY: test
test: FORCE
	bazel test //test/...
